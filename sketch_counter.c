#include "picotiny_hal.h"

int count = 0;

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("Binary counter 0-63!");
}

void loop() {
    // show count in binary on 6 LEDs
    for (int i = 0; i < 6; i++)
        digitalWrite(i, bitRead(count, i) ? LOW : HIGH);
    delay(250);
    count = (count + 1) & 0x3F;
    Serial.print_int(count);
    Serial.println("");
}
