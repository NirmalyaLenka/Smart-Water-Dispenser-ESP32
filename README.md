# Smart Water Dispenser — ESP32

A prototype water dispenser controller using the ESP32's capacitive-touch input, an active-low IR proximity sensor, and a relay-controlled DC pump. Pump run time is estimated from a calibration value; there is no flow meter in the current firmware.

## Current behavior

| Input | Firmware behavior |
|---|---|
| One touch, with no second touch inside the 500 ms window | Run the pump for an estimated 300 mL |
| Second touch inside the 500 ms window | Run the pump for an estimated 1 L |
| IR output remains LOW for about 800 ms while idle | Run the pump for an estimated 300 mL |
| Dispense finishes | Switch pump off and wait through a 5 s cooldown |

The pump is controlled with **GPIO 26 HIGH = ON** and LOW = OFF. GPIO 2 is used as the status LED. The volumes are estimates derived from the configured pump rate, not measured delivery amounts.

## Hardware and pinout

| ESP32 pin | Connection | Notes |
|---|---|---|
| GPIO 4 / touch channel T0 | Touch pad | Capacitive touch input |
| GPIO 14 | IR sensor OUT | Active LOW; firmware uses `INPUT_PULLUP` |
| GPIO 26 | Relay IN | Firmware assumes active-HIGH relay input |
| GPIO 2 | Status LED | Built-in LED on supported boards |
| GND | Sensor/logic ground | Follow the wiring diagram and relay module requirements |

See [`hardware/wiring_diagram.svg`](hardware/wiring_diagram.svg) and [`state_machine.md`](state_machine.md). The repository also has a root-level `wiring_diagram.svg` and duplicate BOM files; they have been left in place.

### Safety before powering

- This documentation is for a **low-voltage DC pump prototype**. Do not use the project to switch mains voltage.
- Keep water, tubing, and wet hands away from the ESP32, USB connector, and exposed wiring. Use an enclosure and suitable strain relief for any real installation.
- Use a separate, correctly rated pump supply where required by the pump; check the relay contact rating, startup current, polarity, and wiring against the actual modules.
- Verify relay OFF behavior with the pump disconnected first. Then test with water in a container, watch for leaks, and never run a pump dry unless its datasheet permits it.
- Time-based dispensing can overfill or underfill when supply voltage, head height, tubing, or pump condition changes. Do not use it for a safety-critical or hygienic dispensing application without independent safeguards.

## Software prerequisites

- Arduino IDE 2.x.
- The **ESP32 by Espressif Systems** board package.
- USB data cable and the correct serial port.

The current sketch includes only Arduino/ESP32 core headers; no third-party library is referenced in `smart_water_dispenser.ino`. A `platformio.ini` is not present, so this README does not claim a working PlatformIO build path.

## Build and upload

1. Review the BOM and wiring diagram before connecting the pump.
2. Open the root-level `smart_water_dispenser.ino` in Arduino IDE.
3. Select the matching ESP32 board (the sketch header names **ESP32 Dev Module**) and the correct serial port.
4. Compile and upload through USB.
5. Open Serial Monitor at **115200 baud**.
6. With the pump disconnected, confirm startup messages and that the relay output is OFF. Test touch and IR triggers before connecting the pump.
7. Calibrate dispensing into a graduated container, then repeat tests for single touch, double touch, IR trigger, and cooldown.

No firmware build or physical test was performed during this documentation update.

## Calibration and settings

The current firmware keeps its operational constants directly in `smart_water_dispenser.ino`:

| Setting in sketch | Current value | Meaning |
|---|---:|---|
| `TOUCH_THRESHOLD` | 40 | Touch readings below this value count as touched |
| `DOUBLE_TAP_WINDOW` | 500 ms | Time allowed for the second tap |
| `IR_STABLE_MS` | 800 ms | Required stable IR detection |
| `IR_COOLDOWN_MS` | 5000 ms | Post-dispense cooldown |
| `DURATION_300` | about 5700 ms | Estimated 300 mL pump run time |
| `DURATION_1000` | 19000 ms | Estimated 1 L pump run time |

**Important configuration limitation:** `config.h` contains matching, user-facing settings, but the current `.ino` does not include that header; it hardcodes the values above. Editing `config.h` alone will not change firmware behavior. The header and sketch have not been refactored in this documentation-only pass.

To calibrate volume, collect and measure the pump output for a known duration, calculate the actual flow rate, and adjust the constants in the sketch deliberately. Re-test both dispense modes after any adjustment.

## Project files

| Path | Purpose |
|---|---|
| `smart_water_dispenser.ino` | ESP32 firmware and state machine |
| `config.h` | Intended user settings; currently not included by the sketch |
| `BOM.md` | Root-level bill of materials |
| `hardware/BOM.md` | Duplicate BOM copy |
| `hardware/wiring_diagram.svg` | Wiring illustration |
| `wiring_diagram.svg` | Root-level duplicate wiring illustration |
| `state_machine.md` | State and timing explanation |
| `index.html` | Browser demonstration; does not control physical hardware |
| `LICENSE` | Project license |

## Troubleshooting

- **Touch input is unreliable:** compare raw touch readings in a temporary diagnostic sketch and adjust the threshold carefully.
- **IR triggers constantly:** check sensor polarity, wiring, and the module's sensitivity adjustment.
- **Relay logic is inverted:** the current firmware assumes active-HIGH; verify the exact relay board before connecting the pump.
- **ESP32 resets when the pump starts:** check pump startup current, supply capacity, grounding/relay wiring, and transient suppression appropriate to the load.
- **Delivered volume is wrong:** recalibrate using measured output; the firmware has no flow feedback.
- **Pump will not stop:** disconnect its power immediately and inspect relay wiring before further testing.

## Limitations and verification

- No flow sensor, closed-loop volume control, leak detection, or independent overfill cutoff is implemented.
- `config.h` is not wired into the current sketch.
- The browser page is a demo only; it is not a connected control dashboard.
- Documentation was checked against the current sketch, configuration header, BOM, and state-machine notes. Compilation and hardware behavior remain unverified.

## License

See [`LICENSE`](LICENSE).

## Maintenance & audit

- **Audit date:** 11 November 2026
- **Scope:** documentation-only review; firmware, pins, and hardware behavior were not changed.
- **Verification:** build and physical operation were not tested.
