// Demonstrates: random(), constrain(), millis(), bitRead()
#include "picotiny_hal.h"

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    randomSeed(12345);
    Serial.println("Random LEDs!");
}

void loop() {
    int pattern = random(0, 64);       // random 6-bit number
    for (int i = 0; i < 6; i++)
        digitalWrite(i, bitRead(pattern, i) ? LOW : HIGH);

    int wait = constrain(random(50, 500), 50, 500);
    delay(wait);

    Serial.print("pattern=");
    Serial.print_int(pattern);
    Serial.print("  wait=");
    Serial.print_int(wait);
    Serial.print("ms  t=");
    Serial.print_int(millis());
    Serial.println("ms");
}
