/*
  PWMFade — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Fades all 6 LEDs in and out using software PWM.
  Demonstrates: analogWrite(), map()
*/

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("PWM Fade!");
}

void loop() {
    // Fade in
    for (int b = 0; b <= 255; b += 5) {
        for (int i = 0; i < 6; i++) analogWrite(i, b);
    }
    // Fade out
    for (int b = 255; b >= 0; b -= 5) {
        for (int i = 0; i < 6; i++) analogWrite(i, b);
    }
}
