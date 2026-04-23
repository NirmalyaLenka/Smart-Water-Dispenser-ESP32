# 💧 Smart Water Dispenser — ESP32

> Capacitive-touch + IR-proximity water dispenser with relay-controlled pump.  
> Single tap = 300 mL · Double tap = 1 L · Auto IR = 300 mL

---

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Wiring Diagram](#wiring-diagram)
- [Pin Reference](#pin-reference)
- [Flow Logic](#flow-logic)
- [Pump Calibration](#pump-calibration)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## Features

| Mode | Trigger | Volume |
|------|---------|--------|
| Single tap | 1× touch on sensor | 300 mL |
| Double tap | 2× touch within 500 ms | 1000 mL (1 L) |
| Auto (IR) | Object held in front of IR sensor | 300 mL |

- ⚡ ESP32 capacitive touch — **no mechanical button**
- 🔍 IR proximity sensor auto-detects cup/bottle
- 🔌 Relay module safely switches pump (mains or high-current DC)
- ⏱️ Precise time-based volume control (calibrated to your pump)
- 🔒 Post-dispense cooldown prevents accidental re-trigger

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Dev Module (any 38-pin variant) |
| Water Pump | 5 V / 12 V mini submersible or peristaltic (1 L / 19 s) |
| Relay Module | 5 V, 1-channel, optocoupler isolated |
| Touch Sensor | Capacitive pad wired to ESP32 GPIO 4 (TOUCH0) |
| IR Sensor | FC-51 or equivalent (active-LOW digital output) |
| Power Supply | 5 V 2 A for ESP32 + relay logic; separate rail for pump |

---

## Wiring Diagram

See [`hardware/wiring_diagram.svg`](hardware/wiring_diagram.svg) for the full schematic.

```
ESP32                      Peripherals
─────────────────────────────────────────────
GPIO 4  (TOUCH0) ──────── Touch Pad (copper foil or dedicated module)
GPIO 14          ──────── IR Sensor OUT  (pull-up via INPUT_PULLUP)
GPIO 26          ──────── Relay IN
GPIO 2  (LED)    ──────── Built-in LED (status indicator)
GND              ──────── IR GND, Relay GND, common ground
3.3 V            ──────── IR VCC
5 V (Vin)        ──────── Relay VCC

Relay COM ─── Pump +V supply
Relay NO  ─── Pump + terminal
Pump –    ─── Power Supply GND
```

---

## Pin Reference

| GPIO | Function | Mode | Notes |
|------|----------|------|-------|
| 4 | Touch sensor | INPUT (capacitive) | `touchRead()` — threshold < 40 |
| 14 | IR sensor OUT | INPUT_PULLUP | LOW = object present |
| 26 | Relay IN | OUTPUT | HIGH = pump ON |
| 2 | Status LED | OUTPUT | Built-in; ON during dispense |

---

## Flow Logic

```
            ┌─────────┐
            │  IDLE   │
            └────┬────┘
                 │
     ┌───────────┼───────────┐
     ▼           ▼           ▼
  Touch?       Touch?      IR stable
 (rising)    (rising)    (800 ms)?
     │            │           │
     ▼            │           ▼
WAIT_2ND_TAP      │       Dispense
 (500 ms window)  │       300 mL
     │            │
 2nd tap?    No 2nd tap?
     │            │
     ▼            ▼
Dispense      Dispense
 1000 mL       300 mL
     │            │
     └──────┬─────┘
            ▼
       DISPENSING
       (timed run)
            │
            ▼
        COOLDOWN
        (5 000 ms)
            │
            ▼
          IDLE
```

---

## Pump Calibration

The firmware uses **time-based** volume control:

```
mL/ms = PUMP_CALIBRATION_ML / PUMP_CALIBRATION_MS
      = 1000 mL / 19 000 ms
      ≈ 0.05263 mL/ms

Duration for 300 mL = 300 / 0.05263 ≈ 5 700 ms
Duration for 1 L    = 19 000 ms (exact)
```

To recalibrate for your pump:
1. Run the pump into a measuring cup for exactly 10 seconds.
2. Note the volume in mL.
3. Update `config.h`:
   ```c
   #define PUMP_CALIBRATION_ML   <your_ml>
   #define PUMP_CALIBRATION_MS   10000UL
   ```

---

## Getting Started

### Prerequisites
- [Arduino IDE 2.x](https://www.arduino.cc/en/software) **or** [PlatformIO](https://platformio.org/)
- ESP32 board package: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

### Flash

```bash
# Arduino IDE
# 1. Board  → ESP32 Dev Module
# 2. Upload Speed → 115200
# 3. Open firmware/smart_water_dispenser.ino
# 4. Click Upload

# PlatformIO (CLI)
pio run --target upload --upload-port /dev/ttyUSB0
```

### Serial Monitor
Open at **115200 baud** to see real-time pump events:
```
[TOUCH] First tap detected — waiting for second...
[TOUCH] Single tap — dispense 300 mL
[DISPENSE] Single Tap → 300 mL (5.7 s)
[PUMP] ON
[PUMP] OFF
[STATE] Cooldown
[STATE] Idle — ready
```

---

## Project Structure

```
smart-water-dispenser/
├── firmware/
│   ├── smart_water_dispenser.ino   # Main ESP32 sketch
│   └── config.h                    # All tunable constants
├── hardware/
│   ├── wiring_diagram.svg          # Schematic / wiring diagram
│   └── BOM.md                      # Bill of Materials
├── docs/
│   └── state_machine.md            # Detailed state machine documentation
├── demo/
│   └── index.html                  # Interactive web demonstration
├── README.md
└── LICENSE
```

---

## Configuration

All user-tunable constants live in `firmware/config.h`:

| Constant | Default | Description |
|----------|---------|-------------|
| `TOUCH_THRESHOLD_VALUE` | 40 | Capacitive reading below which = touched |
| `DOUBLE_TAP_WINDOW_MS` | 500 | Max ms between two taps to count as double |
| `IR_STABLE_DELAY_MS` | 800 | How long object must be present before auto-dispense |
| `POST_DISPENSE_COOLDOWN_MS` | 5000 | Cooldown after dispense before IR re-arms |
| `PUMP_CALIBRATION_ML` | 1000 | Known volume for calibration |
| `PUMP_CALIBRATION_MS` | 19000 | Time to deliver calibration volume |
| `RELAY_ACTIVE_STATE` | HIGH | Change to LOW for active-LOW relay boards |

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Touch never fires | Threshold too high | Lower `TOUCH_THRESHOLD_VALUE` in config.h |
| Double tap never triggers 1 L | Tapping too slow | Increase `DOUBLE_TAP_WINDOW_MS` to 700 |
| IR auto-dispenses in empty space | IR sensitivity too high | Adjust pot on FC-51 module |
| Pump runs too long / too short | Wrong calibration | Re-measure pump and update config.h |
| Relay chatters | Power supply noise | Add 100 µF cap across relay coil |
| ESP32 resets during pump start | Insufficient current | Use separate 5 V rail for relay/pump |

---

## License

MIT License — see [LICENSE](LICENSE) for full text.  
Built with ❤️ using ESP32 Arduino Core.
2
