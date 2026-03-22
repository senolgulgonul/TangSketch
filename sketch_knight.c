#include "picotiny_hal.h"

int pos = 0;
int spd = 100;   // ms per step

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("Knight Rider!");
}

void loop() {
    static int dir = 1;
    for (int i = 0; i < 6; i++) digitalWrite(i, HIGH);  // all OFF
    digitalWrite(pos, LOW);                               // current ON
    delay(spd);
    pos += dir;
    if (pos >= 5 || pos <= 0) dir = -dir;
}
