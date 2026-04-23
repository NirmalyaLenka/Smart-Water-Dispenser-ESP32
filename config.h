#pragma once
/**
 * config.h — User-tunable parameters for Smart Water Dispenser
 * Edit these values to match your hardware without touching main logic.
 */

// ─── Touch Sensitivity ──────────────────────────────────────
// Lower value = more sensitive. Typical air reading ≈ 60–80; touch ≈ 10–25.
#define TOUCH_THRESHOLD_VALUE    40

// ─── Double-Tap Window (ms) ──────────────────────────────────
// How quickly you must tap twice to trigger 1 L mode.
#define DOUBLE_TAP_WINDOW_MS     500

// ─── IR Sensor Stability Delay (ms) ─────────────────────────
// Object must remain in front of IR for this long before auto-dispensing.
#define IR_STABLE_DELAY_MS       800

// ─── Post-Dispense Cooldown (ms) ────────────────────────────
// Prevents the IR sensor from immediately re-triggering after a dispense.
#define POST_DISPENSE_COOLDOWN_MS  5000

// ─── Pump Flow Rate ──────────────────────────────────────────
// Measure your actual pump: time (ms) to deliver PUMP_CALIBRATION_ML mL.
#define PUMP_CALIBRATION_ML      1000.0f
#define PUMP_CALIBRATION_MS      19000UL

// ─── Relay Logic ─────────────────────────────────────────────
// Set to LOW if your relay module is active-LOW.
#define RELAY_ACTIVE_STATE       HIGH

// ─── Debug Serial ────────────────────────────────────────────
#define SERIAL_BAUD_RATE         115200
