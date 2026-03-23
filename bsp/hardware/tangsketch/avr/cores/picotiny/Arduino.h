#pragma once
// Arduino.h for TangSketch (Tang Nano 9K / PicoRV32)

#include <stdint.h>
#include <stddef.h>

// ── Peripherals ───────────────────────────────────────────────
#define _GPIO_OUT  (*(volatile uint32_t*)0x82000000)
#define _GPIO_IN   (*(volatile uint32_t*)0x82000004)
#define _GPIO_OE   (*(volatile uint32_t*)0x82000008)
#define _UART_DATA (*(volatile uint32_t*)0x83000000)
#define _UART_DIV  (*(volatile uint32_t*)0x83000004)

#ifndef F_CPU
#define F_CPU  25175000UL
#endif
#define _UART_DIV_115200  216

// ── Arduino constants ─────────────────────────────────────────
#define HIGH         1
#define LOW          0
#define OUTPUT       1
#define INPUT        0
#define INPUT_PULLUP 0
#define LED_BUILTIN  0
#define MSBFIRST     1
#define LSBFIRST     0
#define PI           3.14159265f

typedef uint8_t  byte;
typedef uint8_t  boolean;

// ── Time ──────────────────────────────────────────────────────
static volatile uint32_t _us_count = 0;

static inline void delay(uint32_t ms) {
    volatile int i;
    for (uint32_t m = 0; m < ms; m++) {
        for (i = 0; i < 20; i++);
        _us_count += 1000;
    }
}
static inline void delayMicroseconds(uint32_t us) {
    volatile uint32_t n = us * 6; while(n--);
    _us_count += us;
}
static inline uint32_t micros(void) { return _us_count; }
static inline uint32_t millis(void) { return _us_count / 1000; }

// ── Digital I/O ───────────────────────────────────────────────
static inline void pinMode(int pin, int mode) {
    if (mode == OUTPUT) { _GPIO_OE |= (1u<<pin); _GPIO_OUT |= (1u<<pin); }
    else                  _GPIO_OE &= ~(1u<<pin);
}
static inline void digitalWrite(int pin, int val) {
    if (val == HIGH) _GPIO_OUT |=  (1u<<pin);
    else             _GPIO_OUT &= ~(1u<<pin);
}
static inline int digitalRead(int pin) {
    return ((_GPIO_IN >> pin) & 1) ? LOW : HIGH;
}

// ── Analog ────────────────────────────────────────────────────
static inline void analogWrite(int pin, int val) {
    for (int i=0; i<255; i++) {
        if (i<val) _GPIO_OUT &= ~(1u<<pin);
        else       _GPIO_OUT |=  (1u<<pin);
        delayMicroseconds(4);
    }
}
#define analogRead(pin) (0)

// ── Math ──────────────────────────────────────────────────────
#undef abs
#define abs(x)              ((x)>=0?(x):-(x))
#define min(a,b)            ((a)<(b)?(a):(b))
#define max(a,b)            ((a)>(b)?(a):(b))
#define constrain(x,lo,hi)  ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))
#define sq(x)               ((x)*(x))

static inline int32_t map(int32_t v,int32_t il,int32_t ih,int32_t ol,int32_t oh){
    return ol+(v-il)*(oh-ol)/(ih-il);
}
static uint32_t _rng=1;
static inline void randomSeed(uint32_t s){_rng=s?s:1;}
static inline int32_t random(int32_t lo,int32_t hi){
    _rng^=_rng<<13;_rng^=_rng>>17;_rng^=_rng<<5;
    int32_t r=hi-lo;if(r<=0)return lo;
    return lo+(int32_t)(_rng%(uint32_t)r);
}

// ── Bits & Bytes ──────────────────────────────────────────────
#define bit(n)              (1UL<<(n))
#define bitRead(v,n)        (((v)>>(n))&1)
#define bitSet(v,n)         ((v)|=(1UL<<(n)))
#define bitClear(v,n)       ((v)&=~(1UL<<(n)))
#define bitWrite(v,n,b)     ((b)?bitSet(v,n):bitClear(v,n))
#define lowByte(w)          ((uint8_t)((w)&0xFF))
#define highByte(w)         ((uint8_t)(((w)>>8)&0xFF))

// ── Characters ───────────────────────────────────────────────
#define isDigit(c)          ((c)>='0'&&(c)<='9')
#define isAlpha(c)          (((c)>='a'&&(c)<='z')||((c)>='A'&&(c)<='Z'))
#define isAlphaNumeric(c)   (isAlpha(c)||isDigit(c))
#define isUpperCase(c)      ((c)>='A'&&(c)<='Z')
#define isLowerCase(c)      ((c)>='a'&&(c)<='z')
#define isSpace(c)          ((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\r')
#define isPrintable(c)      ((c)>=32&&(c)<127)
#define isAscii(c)          ((c)>=0&&(c)<=127)
#define isControl(c)        ((c)<32||(c)==127)

// ── Advanced I/O ─────────────────────────────────────────────
static inline void shiftOut(int dp,int cp,int order,uint8_t val){
    for(int i=0;i<8;i++){
        int b=(order==MSBFIRST)?(7-i):i;
        digitalWrite(dp,(val>>b)&1?HIGH:LOW);
        digitalWrite(cp,HIGH);delayMicroseconds(1);
        digitalWrite(cp,LOW); delayMicroseconds(1);
    }
}
static inline uint8_t shiftIn(int dp,int cp,int order){
    uint8_t v=0;
    for(int i=0;i<8;i++){
        digitalWrite(cp,HIGH);delayMicroseconds(1);
        int b=(order==MSBFIRST)?(7-i):i;
        if(digitalRead(dp)) v|=(1<<b);
        digitalWrite(cp,LOW);delayMicroseconds(1);
    }
    return v;
}
static inline uint32_t pulseIn(int pin,int state,uint32_t timeout){
    uint32_t t=micros();
    while(digitalRead(pin)==state)  if(micros()-t>timeout)return 0;
    while(digitalRead(pin)!=state)  if(micros()-t>timeout)return 0;
    uint32_t ps=micros();
    while(digitalRead(pin)==state)  if(micros()-t>timeout)return 0;
    return micros()-ps;
}

// ── Interrupts (stubs) ────────────────────────────────────────
#define interrupts()    do{}while(0)
#define noInterrupts()  do{}while(0)

// ── Serial ────────────────────────────────────────────────────
static inline void _pc(char c){if(c=='\n')_UART_DATA='\r';_UART_DATA=c;}
static inline void _ps(const char *s){while(*s)_pc(*s++);}
static inline void _pi(int32_t v){
    if(v<0){_pc('-');v=-v;}
    if(v==0){_pc('0');return;}
    char b[12];int i=0;
    while(v>0){int32_t r=v,q=0;while(r>=10){r-=10;q++;}b[i++]='0'+r;v=q;}
    while(i--)_pc(b[i]);
}
static inline void _ph(uint32_t v){
    _ps("0x");
    for(int i=28;i>=0;i-=4){uint8_t n=(v>>i)&0xF;_pc(n<10?'0'+n:'a'+n-10);}
}

// C++ class — works in Arduino IDE (compiles as C++)
class HardwareSerial {
public:
    void begin(uint32_t baud)   { (void)baud; _UART_DIV=_UART_DIV_115200; }
    void print(const char *s)   { _ps(s); }
    void println(const char *s) { _ps(s); _pc('\n'); }
    void print(int32_t v)       { _pi(v); }
    void println(int32_t v)     { _pi(v); _pc('\n'); }
    void print(uint32_t v)      { _pi((int32_t)v); }
    void println(uint32_t v)    { _pi((int32_t)v); _pc('\n'); }
    void print(int v)           { _pi((int32_t)v); }
    void println(int v)         { _pi((int32_t)v); _pc('\n'); }
    void print(char c)          { _pc(c); }
    void println(char c)        { _pc(c); _pc('\n'); }
    void println()              { _pc('\n'); }
    void write(uint8_t c)       { _pc(c); }
    int  available()            { return 0; }
    int  read()                 { return (int)(_UART_DATA&0xFF); }
    void printHex(uint32_t v)   { _ph(v); }
};

extern HardwareSerial Serial;
