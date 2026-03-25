/*
  SerialEcho — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Echoes characters received over Serial back to the terminal.
  Also blinks LED0 on each received character.

  Open Serial Monitor at 115200 baud and type anything.
*/

void setup() {
    Serial.begin(115200);
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);  // LED OFF
    Serial.println("Serial Echo ready — type something!");
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        Serial.print("Echo: ");
        Serial.println_int(c);
        // Blink LED0 on receive
        digitalWrite(0, LOW);
        delay(50);
        digitalWrite(0, HIGH);
    }
}
