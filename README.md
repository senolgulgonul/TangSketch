# TangSketch 🎯

**Write Arduino sketches for the Tang Nano 9K FPGA — running on a PicoRV32 RISC-V soft core.**

No Arduino IDE. No special toolchain magic. Just write `setup()` and `loop()`, compile with RISC-V GCC, and flash over UART in seconds.

```c
#include "picotiny_hal.h"

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello from FPGA!");
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);   // LED ON
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);  // LED OFF
    delay(500);
}
```

---

## How It Works

```
Arduino C sketch
      ↓
riscv-none-elf-gcc  (-march=rv32i)
      ↓
RV32I binary (.elf)
      ↓
make_flash.py  → .v hex file
      ↓
pico-programmer.py  → UART flash
      ↓
PicoRV32 CPU running inside GW1NR-9C FPGA
```

The Tang Nano 9K FPGA runs the **picotiny SoC** — a PicoRV32 RISC-V CPU core synthesized in Verilog using Gowin EDA. `TangSketch` gives that CPU an Arduino-compatible C library so you can write familiar sketches instead of raw register code.

---

## Prerequisites

### 1. Tang Nano 9K with picotiny bitstream
Flash the picotiny SoC bitstream to your board using Gowin EDA:
- Clone: `https://github.com/sipeed/TangNano-9K-example`
- Open `picotiny/project/picotiny.gprj` in Gowin EDA v1.9.12
- Synthesize → Place & Route → flash `impl/pnr/picotiny.fs` to **embFlash**

### 2. xPack RISC-V GCC (Windows)
Download from: `https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/latest`  
Unzip and add `bin/` to your PATH.  
Verify: `riscv-none-elf-gcc --version`

### 3. Python 3 + pyserial
```
py -m pip install pyserial
```

---

## Quick Start

```
git clone https://github.com/senolgulgonul/TangSketch
cd TangSketch
```

**Compile a sketch:**
```cmd
py make_flash.py sketch_blink.c
```

**Flash to Tang Nano 9K:**
```cmd
py ..\sw\pico-programmer.py sketch_blink_flash.v COM4
```
Press the **S1 button** on the board when you see `- Waiting for reset -`

Open a serial terminal at **115200 baud** to see output.

> **Note:** Replace `COM4` with your actual COM port (check Device Manager → Ports)  
> `pico-programmer.py` is in `TangNano-9K-example/picotiny/sw/`

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
| Serial | `Serial.begin`, `Serial.print`, `Serial.println`, `Serial.print_int`, `Serial.print_hex`, `Serial.write`, `Serial.read` |
| Constants | `HIGH`, `LOW`, `INPUT`, `OUTPUT`, `LED_BUILTIN`, `MSBFIRST`, `LSBFIRST`, `true`, `false` |

### ⚠️ Important Differences from Arduino

| | Arduino | Tang Nano 9K |
|---|---|---|
| LED logic | Active HIGH | **Active LOW** — `LOW` = ON, `HIGH` = OFF |
| Print integers | `Serial.print(42)` | `Serial.print_int(42)` |
| ADC | `analogRead()` | ❌ Not available (no ADC hardware) |
| Tone | `tone()` | ❌ Not available |
| I2C / SPI | `Wire`, `SPI` | ❌ Not in picotiny SoC |

---

## Example Sketches

| File | Description |
|---|---|
| `sketch_blink.c` | LED blink every 500ms + Serial output |
| `sketch_knight.c` | Knight Rider / KITT scanner on 6 LEDs |
| `sketch_counter.c` | Binary counter 0-63 on 6 LEDs |
| `sketch_random_leds.c` | Random patterns using `random()`, `millis()` |
| `sketch_fade.c` | PWM fade in/out using `analogWrite()` |

---

## Hardware

**Board:** Sipeed Tang Nano 9K  
**FPGA:** Gowin GW1NR-9C (8640 LUT4)  
**CPU:** PicoRV32 RV32I soft core @ 25.175 MHz  
**LEDs:** 6 × active-low (pins 0–5)  
**Buttons:** 2 × active-low (S1=reset, S2=GPIO)  
**UART:** 115200 baud via BL702 USB bridge  

---

## File Structure

```
TangSketch/
├── picotiny_hal.h        ← Arduino HAL — include this in your sketch
├── crt.S                 ← Startup assembly (from picotiny fw-flash)
├── sections.lds          ← Linker script (from picotiny fw-flash)
├── make_flash.py         ← Build + convert tool
├── sketch_blink.c
├── sketch_knight.c
├── sketch_counter.c
├── sketch_random_leds.c
└── sketch_fade.c
```

---

## Author

**Senol Gulgonul**  
GitHub: [@senolgulgonul](https://github.com/senolgulgonul)

Built with ❤️ for the FPGA + maker community.  
If this helped you, give it a ⭐ and share it!

---

## License

MIT License — free to use, modify, and share.
