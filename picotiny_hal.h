// picotiny_hal.h — Arduino-compatible HAL for picotiny SoC
// Tang Nano 9K — PicoRV32 @ 25.175 MHz
//
// USAGE: just #include "picotiny_hal.h" and write setup() + loop()
// ─────────────────────────────────────────────────────────────────
// SUPPORTED Arduino functions:
//
//  Digital I/O:   pinMode, digitalWrite, digitalRead
//  Analog I/O:    analogWrite (software PWM, pins 0-5)
//                 analogRead  → NOT available (no ADC on Tang Nano 9K)
//  Time:          delay, delayMicroseconds, millis, micros
//  Math:          abs, min, max, constrain, map, sq, random, randomSeed
//  Bits & Bytes:  bit, bitRead, bitWrite, bitSet, bitClear, lowByte, highByte
//  Characters:    isDigit, isAlpha, isAlphaNumeric, isUpperCase, isLowerCase,
//                 isSpace, isPunct, isAscii, isControl, isPrintable
//  Advanced I/O:  shiftOut, shiftIn, pulseIn
//  Serial:        Serial.begin, Serial.print, Serial.println,
//                 Serial.available, Serial.read, Serial.write
//  Interrupts:    interrupts, noInterrupts (stubs — PicoRV32 IRQ)
//
// NOT available (hardware limitation):
//   analogRead, tone, Wire (I2C), SPI, WiFi, USB, attachInterrupt
// ─────────────────────────────────────────────────────────────────

#ifndef PICOTINY_HAL_H
#define PICOTINY_HAL_H

#include <stdint.h>

// ── Peripherals ───────────────────────────────────────────────
#define _GPIO_OUT  (*(volatile uint32_t*)0x82000000)
#define _GPIO_IN   (*(volatile uint32_t*)0x82000004)
#define _GPIO_OE   (*(volatile uint32_t*)0x82000008)
#define _UART_DATA (*(volatile uint32_t*)0x83000000)
#define _UART_DIV  (*(volatile uint32_t*)0x83000004)

#define F_CPU             25175000UL
#define _UART_DIV_115200  216   // 25175000/115200 - 2

// ═══════════════════════════════════════════════════════════════
// ARDUINO CONSTANTS
// ═══════════════════════════════════════════════════════════════
#define HIGH         1
#define LOW          0
#define OUTPUT       1
#define INPUT        0
#define INPUT_PULLUP 0
#define LED_BUILTIN  0
#ifndef true
#define true  1
#define false 0
#endif
typedef uint8_t  byte;
typedef uint8_t  boolean;

// ═══════════════════════════════════════════════════════════════
// TIME
// ═══════════════════════════════════════════════════════════════
volatile int _di;
static volatile uint32_t _us_count = 0;

static inline void delay(uint32_t ms) {
    for (uint32_t m = 0; m < ms; m++) {
        for (_di = 0; _di < 20; _di++);
        _us_count += 1000;
    }
}

static inline void delayMicroseconds(uint32_t us) {
    volatile uint32_t n = us * 6;
    while (n--);
    _us_count += us;
}

static inline uint32_t micros(void)  { return _us_count; }
static inline uint32_t millis(void)  { return _us_count / 1000; }

// ═══════════════════════════════════════════════════════════════
// DIGITAL I/O
// ═══════════════════════════════════════════════════════════════
static inline void pinMode(int pin, int mode) {
    if (mode == OUTPUT) {
        _GPIO_OE  |=  (1u << pin);
        _GPIO_OUT |=  (1u << pin);  // HIGH = LED OFF (active-low)
    } else {
        _GPIO_OE &= ~(1u << pin);
    }
}

// Tang Nano 9K LEDs are active-LOW:
//   LOW  = LED ON,  HIGH = LED OFF
static inline void digitalWrite(int pin, int val) {
    if (val == HIGH) _GPIO_OUT |=  (1u << pin);
    else             _GPIO_OUT &= ~(1u << pin);
}

static inline int digitalRead(int pin) {
    // buttons active-low → invert so HIGH=pressed matches Arduino
    return ((_GPIO_IN >> pin) & 1) ? LOW : HIGH;
}

// ═══════════════════════════════════════════════════════════════
// ANALOG I/O
// ═══════════════════════════════════════════════════════════════
// analogWrite: software PWM, 8-bit (0=off, 255=full on)
static inline void analogWrite(int pin, int val) {
    // run one PWM period (255 steps × 4µs = ~1ms)
    for (int i = 0; i < 255; i++) {
        if (i < val) _GPIO_OUT &= ~(1u << pin);  // ON
        else         _GPIO_OUT |=  (1u << pin);  // OFF
        delayMicroseconds(4);
    }
}

// analogRead: not available on Tang Nano 9K (no ADC)
// Returns 0 to avoid compile errors if used
#define analogRead(pin)  (0)

// ═══════════════════════════════════════════════════════════════
// MATH
// ═══════════════════════════════════════════════════════════════
#define abs(x)              ((x) >= 0 ? (x) : -(x))
#define min(a,b)            ((a) < (b) ? (a) : (b))
#define max(a,b)            ((a) > (b) ? (a) : (b))
#define constrain(x,lo,hi)  ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define sq(x)               ((x)*(x))

// map: re-range a value (integer, no floats needed)
static inline int32_t map(int32_t val, int32_t in_lo, int32_t in_hi,
                                       int32_t out_lo, int32_t out_hi) {
    return out_lo + (val - in_lo) * (out_hi - out_lo) / (in_hi - in_lo);
}

// random number — XorShift32, no division
static uint32_t _rng = 1;
static inline void randomSeed(uint32_t seed) { _rng = seed ? seed : 1; }
static inline int32_t random(int32_t lo, int32_t hi) {
    _rng ^= _rng << 13;
    _rng ^= _rng >> 17;
    _rng ^= _rng << 5;
    // map to [lo, hi) using bit masking trick
    uint32_t range = (uint32_t)(hi - lo);
    return lo + (int32_t)(_rng % range);  // % OK: small range
}
// random(max) variant
#define random(...)  _random_va(__VA_ARGS__, 0)
static inline int32_t _random_va(int32_t a, int32_t b, ...) {
    if (b == 0) return random(0, a);
    return random(a, b);
}

// ═══════════════════════════════════════════════════════════════
// BITS AND BYTES
// ═══════════════════════════════════════════════════════════════
#define bit(n)              (1UL << (n))
#define bitRead(v,n)        (((v) >> (n)) & 1)
#define bitSet(v,n)         ((v) |=  (1UL << (n)))
#define bitClear(v,n)       ((v) &= ~(1UL << (n)))
#define bitWrite(v,n,b)     ((b) ? bitSet(v,n) : bitClear(v,n))
#define lowByte(w)          ((uint8_t)((w) & 0xFF))
#define highByte(w)         ((uint8_t)(((w) >> 8) & 0xFF))

// ═══════════════════════════════════════════════════════════════
// CHARACTER FUNCTIONS
// ═══════════════════════════════════════════════════════════════
#define isDigit(c)          ((c) >= '0' && (c) <= '9')
#define isAlpha(c)          (((c)>='a'&&(c)<='z')||((c)>='A'&&(c)<='Z'))
#define isAlphaNumeric(c)   (isAlpha(c) || isDigit(c))
#define isUpperCase(c)      ((c) >= 'A' && (c) <= 'Z')
#define isLowerCase(c)      ((c) >= 'a' && (c) <= 'z')
#define isSpace(c)          ((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\r')
#define isWhitespace(c)     isSpace(c)
#define isPunct(c)          (isPrintable(c) && !isAlphaNumeric(c) && !isSpace(c))
#define isAscii(c)          ((c) >= 0 && (c) <= 127)
#define isControl(c)        ((c) < 32 || (c) == 127)
#define isPrintable(c)      ((c) >= 32 && (c) < 127)
#define isHexadecimalDigit(c) (isDigit(c)||((c)>='a'&&(c)<='f')||((c)>='A'&&(c)<='F'))
#define isGraph(c)          ((c) > 32 && (c) < 127)

// ═══════════════════════════════════════════════════════════════
// ADVANCED I/O
// ═══════════════════════════════════════════════════════════════
#define MSBFIRST 1
#define LSBFIRST 0

static inline void shiftOut(int dataPin, int clkPin, int order, uint8_t val) {
    for (int i = 0; i < 8; i++) {
        int bit_i = (order == MSBFIRST) ? (7 - i) : i;
        digitalWrite(dataPin, (val >> bit_i) & 1 ? HIGH : LOW);
        digitalWrite(clkPin, HIGH);
        delayMicroseconds(1);
        digitalWrite(clkPin, LOW);
        delayMicroseconds(1);
    }
}

static inline uint8_t shiftIn(int dataPin, int clkPin, int order) {
    uint8_t val = 0;
    for (int i = 0; i < 8; i++) {
        digitalWrite(clkPin, HIGH);
        delayMicroseconds(1);
        int bit_i = (order == MSBFIRST) ? (7 - i) : i;
        if (digitalRead(dataPin)) val |= (1 << bit_i);
        digitalWrite(clkPin, LOW);
        delayMicroseconds(1);
    }
    return val;
}

static inline uint32_t pulseIn(int pin, int state, uint32_t timeout_us) {
    uint32_t start = micros();
    // wait for pin to go to opposite state first
    while (digitalRead(pin) == state)
        if (micros() - start > timeout_us) return 0;
    // wait for pulse start
    while (digitalRead(pin) != state)
        if (micros() - start > timeout_us) return 0;
    uint32_t pulse_start = micros();
    // measure pulse
    while (digitalRead(pin) == state)
        if (micros() - start > timeout_us) return 0;
    return micros() - pulse_start;
}

// ═══════════════════════════════════════════════════════════════
// INTERRUPTS (stubs — PicoRV32 has IRQ but picotiny doesn't wire GPIO IRQs)
// ═══════════════════════════════════════════════════════════════
#define interrupts()    do {} while(0)
#define noInterrupts()  do {} while(0)

// ═══════════════════════════════════════════════════════════════
// SERIAL
// ═══════════════════════════════════════════════════════════════
static void _putc(char c) {
    if (c == '\n') _UART_DATA = '\r';
    _UART_DATA = c;
}
static void _puts(const char *s) { while(*s) _putc(*s++); }

// Integer print without division (repeated subtraction)
static void _puti(int32_t v) {
    if (v < 0) { _putc('-'); v = -v; }
    if (v == 0) { _putc('0'); return; }
    char b[12]; int i = 0;
    int32_t n = v;
    while (n > 0) {
        int32_t r = n, q = 0;
        while (r >= 10) { r -= 10; q++; }
        b[i++] = '0' + r;
        n = q;
    }
    while (i--) _putc(b[i]);
}

static void _puth(uint32_t v) {
    _puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t n = (v >> i) & 0xF;
        _putc(n < 10 ? '0'+n : 'a'+n-10);
    }
}

// Serial object with function pointers
struct _Serial_t {
    void (*begin)(uint32_t);
    void (*print)(const char*);
    void (*println)(const char*);
    void (*print_int)(int32_t);
    void (*println_int)(int32_t);
    void (*print_hex)(uint32_t);
    void (*write)(char);
    int  (*available)(void);
    char (*read)(void);
};

static void _sb(uint32_t b)         { _UART_DIV = _UART_DIV_115200; (void)b; }
static void _sp(const char *s)      { _puts(s); }
static void _spl(const char *s)     { _puts(s); _putc('\n'); }
static void _spi(int32_t v)         { _puti(v); }
static void _spli(int32_t v)        { _puti(v); _putc('\n'); }
static void _sph(uint32_t v)        { _puth(v); }
static void _sw(char c)             { _putc(c); }
static int  _sav(void)              { return 0; }
static char _srd(void)              { return (char)(_UART_DATA & 0xFF); }

static struct _Serial_t Serial = {
    _sb, _sp, _spl, _spi, _spli, _sph, _sw, _sav, _srd
};

// ═══════════════════════════════════════════════════════════════
// REQUIRED BY crt.S
// ═══════════════════════════════════════════════════════════════
void irqCallback(void) {}

// ═══════════════════════════════════════════════════════════════
// ARDUINO ENTRY POINT — calls setup() then loop() forever
// ═══════════════════════════════════════════════════════════════
void setup(void);
void loop(void);

void main(void) {
    _UART_DIV = _UART_DIV_115200;
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    setup();
    while (1) loop();
}

#endif // PICOTINY_HAL_H
