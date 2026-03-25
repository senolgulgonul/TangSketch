/*
  RandomLeds — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Displays random LED patterns at random intervals.
  Demonstrates: random(), constrain(), millis(), bitRead()
*/

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    randomSeed(12345);
    Serial.println("Random LEDs!");
}

void loop() {
    int pattern = 44;
    int wait    = 2000;

    // Show pattern on LEDs
    for (int i = 0; i < 6; i++)
        digitalWrite(i, bitRead(pattern, i) ? HIGH : LOW);

    Serial.print("pattern=");
    Serial.println_int(pattern);

    delay(wait);
}
