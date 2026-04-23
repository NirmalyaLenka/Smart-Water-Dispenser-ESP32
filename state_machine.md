# State Machine — Smart Water Dispenser

## States

### IDLE
The default resting state.

**Entry conditions:** System startup, end of COOLDOWN  
**Actions while in state:**
- Continuously reads touch sensor (capacitive)
- Continuously monitors IR sensor

**Transitions:**
| Condition | Next State |
|-----------|-----------|
| Touch rising edge detected | `TOUCH_WAIT_SECOND` |
| IR sensor stable for ≥ 800 ms AND no touch active | `DISPENSING` (300 mL) |

---

### TOUCH_WAIT_SECOND
Entered after the first tap. Waits 500 ms for a second tap.

**Entry condition:** Rising edge on touch sensor from IDLE  
**Timer:** 500 ms window (`DOUBLE_TAP_WINDOW_MS`)

**Transitions:**
| Condition | Next State | Volume |
|-----------|-----------|--------|
| Second touch rising edge within 500 ms | `DISPENSING` | 1000 mL |
| 500 ms elapsed with no second tap | `DISPENSING` | 300 mL |

---

### DISPENSING
Pump is running. Relay is HIGH (pump ON).

**Entry conditions:** From `TOUCH_WAIT_SECOND` or directly from IDLE (IR trigger)  
**Actions:** Relay ON, LED ON, countdown timer active

**Transitions:**
| Condition | Next State |
|-----------|-----------|
| `millis() >= dispenseEndTime` | `COOLDOWN` |

---

### COOLDOWN
Post-dispense rest period. Relay is LOW (pump OFF).

**Entry condition:** End of DISPENSING  
**Duration:** 5 000 ms (`POST_DISPENSE_COOLDOWN_MS`)  
**Purpose:** Prevents IR sensor from immediately re-triggering after a glass is filled and still sits under the sensor.

**Transitions:**
| Condition | Next State |
|-----------|-----------|
| 5 000 ms elapsed | `IDLE` |

---

## Timing Diagram

```
Time ──────────────────────────────────────────────────────────────►

             TAP1           TAP2
Touch   ─────┐  ├────────────┐  ├────────────────────────────────
             │  │            │  │
             │◄─►│           │◄─►│
             50ms debounce   50ms debounce

State   IDLE │ WAIT(500ms) │ DISPENSING(19000ms) │ COOLDOWN(5000ms) │ IDLE
             └─────────────┘                     │
                    ▲ Double tap within 500ms     │
                    └─ 1000 mL selected           └─ pump OFF here


── Auto IR ──────────────────────────────────────────────────────────

IR OUT  HIGH ──────────────────┐    (object arrives)
             LOW ──────────────┴────────────────────────►

             │◄── 800 ms stable ──►│
                                   │
State   IDLE ──────────────────────┤ DISPENSING(5700ms) │ COOLDOWN │ IDLE
                                   └────── pump ON ──────┘
```

---

## Priority

When both touch and IR are active simultaneously:
1. **Touch** takes priority — IR is ignored while a touch event is being processed
2. **IR** only triggers from `IDLE` state, never during touch handling or dispensing
