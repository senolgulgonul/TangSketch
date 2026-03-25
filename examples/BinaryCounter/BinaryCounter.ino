/*
  BinaryCounter — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Counts 0-63 in binary on 6 LEDs.
  LED0 = bit 0 (LSB), LED5 = bit 5 (MSB)
  Prints count value to Serial.
*/

int count = 0;

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("Binary Counter 0-63");
}

void loop() {
    // Display count in binary on LEDs
    for (int i = 0; i < 6; i++)
        digitalWrite(i, bitRead(count, i) ? LOW : HIGH);

    Serial.print("Count: ");
    Serial.println_int(count);

    delay(250);
    count = (count + 1) & 0x3F;  // 6-bit counter
}
