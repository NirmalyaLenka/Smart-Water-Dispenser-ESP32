/**
 * ============================================================
 *  Smart Water Dispenser — ESP32 Firmware
 *  Author : Your Name
 *  Version: 1.0.0
 *  Board  : ESP32 Dev Module
 * ============================================================
 *
 *  FEATURES
 *  --------
 *  • Single touch  → 300 mL
 *  • Double touch  → 1000 mL (1 L)
 *  • IR auto-sense → 300 mL (when no button is pressed)
 *  • Relay-driven pump (1 L / 19 s = ~52.63 mL/s)
 *
 *  WIRING SUMMARY
 *  ---------------
 *  Touch sensor  → GPIO 4   (TOUCH0 / capacitive)
 *  IR sensor OUT → GPIO 14  (active LOW)
 *  Relay IN      → GPIO 26  (active HIGH → pump ON)
 *  Status LED    → GPIO 2   (built-in)
 *
 * ============================================================
 */

#include <Arduino.h>

// ─── Pin Definitions ────────────────────────────────────────
#define TOUCH_PIN         T0       // GPIO 4 – capacitive touch
#define IR_SENSOR_PIN     14       // IR obstacle sensor (LOW = object detected)
#define RELAY_PIN         26       // Relay IN  (HIGH = pump ON)
#define LED_PIN           2        // Built-in LED

// ─── Pump Calibration ───────────────────────────────────────
// Pump delivers 1000 mL in 19 000 ms  → 52.631 mL/s
constexpr float ML_PER_MS        = 1000.0f / 19000.0f;   // ≈ 0.05263 mL/ms
constexpr uint32_t DURATION_300  = (uint32_t)(300.0f / ML_PER_MS);  // ≈ 5 700 ms
constexpr uint32_t DURATION_1000 = 19000UL;                          // exact

// ─── Touch Detection ────────────────────────────────────────
constexpr uint16_t TOUCH_THRESHOLD   = 40;    // Below this value = touched
constexpr uint32_t DOUBLE_TAP_WINDOW = 500;   // ms between taps for double-tap
constexpr uint32_t DEBOUNCE_MS       = 50;

// ─── IR Sensor ──────────────────────────────────────────────
constexpr uint32_t IR_STABLE_MS      = 800;   // Object must be present this long
constexpr uint32_t IR_COOLDOWN_MS    = 5000;  // Prevent re-trigger right after dispense

// ─── State Machine ──────────────────────────────────────────
enum class State {
  IDLE,
  TOUCH_WAIT_SECOND,   // First tap detected, waiting for second
  DISPENSING,
  IR_DETECTED,
  COOLDOWN
};

State       currentState      = State::IDLE;
uint32_t    stateEnteredAt    = 0;
uint32_t    dispenseEndTime   = 0;
uint32_t    lastTouchTime     = 0;
bool        lastTouchState    = false;
uint32_t    irPresentSince    = 0;
bool        irWasPresent      = false;

// ─── Helpers ────────────────────────────────────────────────

bool isTouched() {
  return touchRead(TOUCH_PIN) < TOUCH_THRESHOLD;
}

bool isObjectPresent() {
  return digitalRead(IR_SENSOR_PIN) == LOW;  // Active LOW sensor
}

void pumpOn() {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN,   HIGH);
  Serial.println("[PUMP] ON");
}

void pumpOff() {
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN,   LOW);
  Serial.println("[PUMP] OFF");
}

void startDispense(uint32_t durationMs, const char* reason) {
  Serial.printf("[DISPENSE] %s → %u mL (%.1f s)\n",
    reason,
    (uint32_t)(durationMs * ML_PER_MS),
    durationMs / 1000.0f);
  pumpOn();
  dispenseEndTime = millis() + durationMs;
  currentState    = State::DISPENSING;
  stateEnteredAt  = millis();
}

void enterCooldown() {
  pumpOff();
  currentState   = State::COOLDOWN;
  stateEnteredAt = millis();
  Serial.println("[STATE] Cooldown");
}

// ─── Setup ──────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(IR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN,     OUTPUT);
  pinMode(LED_PIN,       OUTPUT);

  pumpOff();   // ensure relay starts OFF

  Serial.println("===========================================");
  Serial.println("  Smart Water Dispenser — ESP32 v1.0.0");
  Serial.println("===========================================");
  Serial.printf("  300 mL duration : %u ms\n", DURATION_300);
  Serial.printf(" 1000 mL duration : %u ms\n", DURATION_1000);
  Serial.println("  Ready.");
}

// ─── Main Loop ──────────────────────────────────────────────

void loop() {
  uint32_t now         = millis();
  bool     touched     = isTouched();
  bool     objPresent  = isObjectPresent();

  // ── Rising-edge detection for touch ─────────────────────
  bool touchRising = (touched && !lastTouchState);
  lastTouchState   = touched;

  // ════════════════════════════════════════════════════════
  switch (currentState) {

    // ── IDLE ──────────────────────────────────────────────
    case State::IDLE: {
      // Priority 1: touch takes precedence over IR
      if (touchRising) {
        lastTouchTime  = now;
        currentState   = State::TOUCH_WAIT_SECOND;
        stateEnteredAt = now;
        Serial.println("[TOUCH] First tap detected — waiting for second...");
        break;
      }

      // Priority 2: IR auto-dispense
      if (objPresent) {
        if (!irWasPresent) {
          irPresentSince = now;
          irWasPresent   = true;
        } else if ((now - irPresentSince) >= IR_STABLE_MS) {
          Serial.println("[IR] Object stable — auto dispense 300 mL");
          startDispense(DURATION_300, "IR Auto");
          irWasPresent = false;
        }
      } else {
        irWasPresent = false;
      }
      break;
    }

    // ── TOUCH_WAIT_SECOND ─────────────────────────────────
    case State::TOUCH_WAIT_SECOND: {
      if (touchRising) {
        Serial.println("[TOUCH] Second tap — dispense 1000 mL");
        startDispense(DURATION_1000, "Double Tap");
        break;
      }
      // Window expired → single tap = 300 mL
      if ((now - stateEnteredAt) > DOUBLE_TAP_WINDOW) {
        Serial.println("[TOUCH] Single tap — dispense 300 mL");
        startDispense(DURATION_300, "Single Tap");
      }
      break;
    }

    // ── DISPENSING ────────────────────────────────────────
    case State::DISPENSING: {
      if (now >= dispenseEndTime) {
        enterCooldown();
      }
      break;
    }

    // ── COOLDOWN ──────────────────────────────────────────
    case State::COOLDOWN: {
      if ((now - stateEnteredAt) >= IR_COOLDOWN_MS) {
        currentState = State::IDLE;
        irWasPresent = false;
        Serial.println("[STATE] Idle — ready");
      }
      break;
    }
  }
  // ════════════════════════════════════════════════════════

  delay(10);   // small yield; avoids WDT on no-op loops
}
