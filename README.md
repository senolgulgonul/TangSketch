# TangSketch 🎯

**Write Arduino sketches for the Tang Nano 9K FPGA — running on a PicoRV32 RISC-V soft core.**

No Arduino IDE setup hassle. Just install the board package, write `setup()` and `loop()`, and hit Upload.

```cpp
void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello from FPGA!");
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);   // LED ON  (active-low)
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);  // LED OFF
    delay(500);
    Serial.println("blink");
}
```

---

## Install in Arduino IDE 2.x

**Step 1 — Add Boards Manager URL**

File → Preferences → Additional Boards Manager URLs → add:
```
https://raw.githubusercontent.com/senolgulgonul/TangSketch/main/package_tangsketch_index.json
```

**Step 2 — Install the board**

Tools → Board → Boards Manager → search **TangSketch** → Install

**Step 3 — Select board and port**

Tools → Board → TangSketch → **Tang Nano 9K (PicoRV32 / picotiny)**  
Tools → Port → your COM port (e.g. COM4)

**Step 4 — Upload!**

Click Upload — press **S1 button** on the board when prompted.

> ⚠️ **Prerequisite:** The [picotiny SoC bitstream](https://github.com/sipeed/TangNano-9K-example/tree/main/picotiny) must be flashed to your Tang Nano 9K first using Gowin EDA. This is a one-time step.

---

## How It Works

```
Arduino sketch (.ino)
        ↓
riscv-none-elf-gcc  (-march=rv32i)
        ↓
RV32I binary
        ↓
upload.py  →  UART flash  →  Press S1
        ↓
PicoRV32 RISC-V CPU running inside GW1NR-9C FPGA
```

The Tang Nano 9K runs **picotiny** — a PicoRV32 RISC-V SoC synthesized into the FPGA fabric using Gowin EDA. TangSketch provides the Arduino-compatible layer on top.

---

## Prerequisites

### 1. xPack RISC-V GCC (must be in PATH)
Download: https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/latest  
Add `bin/` to your system PATH.  
Verify: `riscv-none-elf-gcc --version`

### 2. Python 3 + pyserial
```
pip install pyserial
```

### 3. picotiny bitstream on Tang Nano 9K
- Clone: https://github.com/sipeed/TangNano-9K-example
- Open `picotiny/project/picotiny.gprj` in Gowin EDA v1.9.12
- Synthesize → Place & Route → flash `impl/pnr/picotiny.fs` to **embFlash**
- This is a **one-time step** — firmware updates go over UART after this

---

## Supported Arduino Functions

| Category | Functions |
|---|---|
| Digital I/O | `pinMode`, `digitalWrite`, `digitalRead` |
| Analog | `analogWrite` (software PWM, 8-bit) |
| Time | `delay`, `delayMicroseconds`, `millis`, `micros` |
| Math | `abs`, `min`, `max`, `constrain`, `map`, `sq`, `random`, `randomSeed` |
| Bits & Bytes | `bit`, `bitRead`, `bitWrite`, `bitSet`, `bitClear`, `lowByte`, `highByte` |
| Characters | `isDigit`, `isAlpha`, `isAlphaNumeric`, `isUpperCase`, `isLowerCase`, `isSpace`, `isPrintable` |
| Advanced I/O | `shiftOut`, `shiftIn`, `pulseIn` |
| Serial | `Serial.begin`, `Serial.print`, `Serial.println`, `Serial.write`, `Serial.read` |

### ⚠️ Key Differences from Standard Arduino

| | Arduino | Tang Nano 9K |
|---|---|---|
| LED logic | Active HIGH | **Active LOW** — `LOW` = ON, `HIGH` = OFF |
| ADC | `analogRead()` available | ❌ No ADC hardware |
| I2C / SPI | `Wire`, `SPI` | ❌ Not in picotiny SoC |
| Tone | `tone()` | ❌ Not available |

---

## Example Sketches

The `arduino/` folder contains ready-to-use examples:

| Sketch | Description |
|---|---|
| `sketch_blink.c` | LED blink every 500ms |
| `sketch_knight.c` | Knight Rider scanner on 6 LEDs |
| `sketch_counter.c` | Binary counter 0–63 on 6 LEDs |
| `sketch_random_leds.c` | Random patterns using `random()` and `millis()` |
| `sketch_fade.c` | PWM fade using `analogWrite()` |

---

## Hardware

| | |
|---|---|
| Board | Sipeed Tang Nano 9K |
| FPGA | Gowin GW1NR-9C (8640 LUT4) |
| CPU | PicoRV32 RV32I soft core @ 25.175 MHz |
| LEDs | 6 × active-low (pins 0–5) |
| Buttons | S1 = reset / ISP trigger, S2 = GPIO |
| UART | 115200 baud via BL702 USB bridge |
| Flash | 32Mbit SPI flash (7MB available for firmware) |

---

## Repository Structure

```
TangSketch/
├── arduino/                  ← Standalone compile tools + sketches
│   ├── picotiny_hal.h        ← Arduino HAL (for command-line use)
│   ├── crt.S                 ← Startup assembly
│   ├── sections.lds          ← Linker script
│   ├── make_flash.py         ← Command-line build tool
│   └── sketch_*.c            ← Example sketches
├── bsp/                      ← Arduino IDE Board Support Package
│   └── hardware/tangsketch/avr/
│       ├── boards.txt
│       ├── platform.txt
│       ├── cores/picotiny/   ← Arduino.h + main.cpp
│       ├── variants/tangnano9k/
│       └── tools/pico_programmer/
├── package_tangsketch_index.json  ← Boards Manager index
└── README.md
```

---

## Credits

- **[Sipeed](https://github.com/sipeed)** — picotiny PicoRV32 SoC for Tang Nano 9K  
  https://github.com/sipeed/TangNano-9K-example
- **[YosysHQ](https://github.com/YosysHQ)** — PicoRV32 RISC-V CPU core  
  https://github.com/YosysHQ/picorv32
- **[Gowin Semiconductor](https://www.gowinsemi.com)** — GW1NR-9C FPGA & EDA tools

---

## Author

**Senol Gulgonul** — [@senolgulgonul](https://github.com/senolgulgonul)

If TangSketch helped you, give it a ⭐ and share it!

---

## License

MIT License — free to use, modify, and share.
