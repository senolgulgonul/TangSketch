// main.cpp — Arduino entry point for TangSketch
#include "Arduino.h"

HardwareSerial Serial;

// Forward declarations — defined in user sketch
void setup(void);
void loop(void);

extern "C" void irqCallback(void) {}

extern "C" void main(void) {
    _UART_DIV = _UART_DIV_115200;
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    setup();
    while (1) loop();
}
