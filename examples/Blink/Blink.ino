/*
  Blink — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Blinks LED0 every 500ms and prints to Serial.

  NOTE: Tang Nano 9K LEDs are active-LOW
    digitalWrite(pin, LOW)  → LED ON
    digitalWrite(pin, HIGH) → LED OFF
*/

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Blink starting!");
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);    // LED ON
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);   // LED OFF
    delay(500);
    Serial.println("blink");
}
