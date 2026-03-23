/*
  ButtonControl — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Press S2 button to change Knight Rider speed.
  Three speeds: slow → medium → fast → slow...

  S2 button is on pin 0 of GPIO_IN (active-low).
  NOTE: S1 is the reset button — do not use S1 as GPIO.
*/

#define BTN_PIN  6   // S2 button
int pos       = 0;

void setup() {
    Serial.begin(115200);
    pinMode(BTN_PIN, INPUT);
    Serial.println("Button Control — press S2!");
}

void loop() {
    int btn = digitalRead(BTN_PIN);
    if (btn==0) digitalWrite(pos, LOW); else digitalWrite(pos, HIGH);
    Serial.print("S2 :");Serial.println(btn);
    delay(100);
}
