#include "picotiny_hal.h"

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Blink ready!");
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);    // LED ON  (active-low)
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);   // LED OFF
    delay(500);
    Serial.println("blink");
}
