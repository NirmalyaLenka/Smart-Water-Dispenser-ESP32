# Bill of Materials (BOM)

Smart Water Dispenser — ESP32

| # | Component | Specification | Qty | Approx. Cost (INR) | Notes |
|---|-----------|--------------|-----|-------------------|-------|
| 1 | ESP32 Dev Module | 38-pin, dual-core 240 MHz | 1 | ₹ 350 | Any clone works |
| 2 | Mini Submersible Pump | 5 V DC, ~1 L/19 s flow rate | 1 | ₹ 120 | Or peristaltic 12 V |
| 3 | Relay Module | 5 V, 1-channel, optocoupler | 1 | ₹ 60 | SRD-05VDC-SL-C |
| 4 | IR Obstacle Sensor | FC-51, adjustable sensitivity | 1 | ₹ 40 | Active-LOW output |
| 5 | Copper Foil / Touch Pad | 40 × 40 mm copper tape | 1 | ₹ 30 | Or TTP223 module |
| 6 | Power Supply | 5 V 2 A USB / wall adapter | 1 | ₹ 150 | Separate for pump |
| 7 | Silicone Tubing | 6 mm ID food-grade | 30 cm | ₹ 50 | Inlet + outlet |
| 8 | Jumper Wires | Male-Female, 20 cm | 10 | ₹ 30 | |
| 9 | Breadboard / PCB | Mini 400-point or custom PCB | 1 | ₹ 40 | |
| 10 | Capacitor | 100 µF 16 V electrolytic | 1 | ₹ 5 | Relay noise filter |
| 11 | LED (optional) | 5 mm red/blue | 1 | ₹ 5 | External indicator |
| 12 | Resistor 330 Ω | 1/4 W | 1 | ₹ 2 | For external LED |
| 13 | Enclosure | 3D-printed or project box | 1 | ₹ 200 | |

**Total Estimated Cost: ~ ₹ 1 082**

## Notes
- If using a **12 V pump**, add a 12 V 2 A power supply and use a relay rated for 12 V switching.
- The ESP32's 3.3 V output is sufficient for the IR sensor VCC.
- The relay module typically requires 5 V for the coil; use ESP32's Vin (USB 5 V) pin.
- Food-grade silicone tubing is **mandatory** for drinking water applications.
