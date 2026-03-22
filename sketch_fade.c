// Demonstrates: analogWrite(), map()
// Fades all 6 LEDs in and out using software PWM
#include "picotiny_hal.h"

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("PWM Fade!");
}

void loop() {
    // fade in
    for (int b = 0; b <= 255; b += 5) {
        for (int i = 0; i < 6; i++) analogWrite(i, b);
    }
    // fade out
    for (int b = 255; b >= 0; b -= 5) {
        for (int i = 0; i < 6; i++) analogWrite(i, b);
    }
}
