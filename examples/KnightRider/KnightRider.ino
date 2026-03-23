/*
  Knight Rider — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Classic KITT scanner effect on all 6 LEDs.
  Change SPEED to adjust the scan rate.
*/

#define SPEED  80   // ms per step (lower = faster)

int pos = 0;
int dir = 1;

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("Knight Rider!");
}

void loop() {
    // Turn all LEDs off
    for (int i = 0; i < 6; i++) digitalWrite(i, HIGH);
    // Light current position
    digitalWrite(pos, LOW);
    delay(SPEED);
    // Move
    pos += dir;
    if (pos >= 5 || pos <= 0) dir = -dir;
}
