/*
  ButtonControl — TangSketch example
  Tang Nano 9K — PicoRV32 RISC-V @ 25.175 MHz

  Press S2 button to change Knight Rider speed.
  Three speeds: slow → medium → fast → slow...

  S2 button is on pin 0 of GPIO_IN (active-low).
  NOTE: S1 is the reset button — do not use S1 as GPIO.
*/

#define BTN_PIN  0   // S2 button

int speeds[]  = {200, 80, 20};
int speedIdx  = 0;
int pos       = 0;
int dir       = 1;
int lastBtn   = HIGH;

void setup() {
    Serial.begin(115200);
    pinMode(BTN_PIN, INPUT);
    for (int i = 0; i < 6; i++) pinMode(i, OUTPUT);
    Serial.println("Button Control — press S2 to change speed!");
}

void loop() {
    // Check button press (debounced)
    int btn = digitalRead(BTN_PIN);
    if (btn == LOW && lastBtn == HIGH) {
        speedIdx = (speedIdx + 1) % 3;
        Serial.print("Speed changed to: ");
        Serial.println(speeds[speedIdx]);
        delay(50);  // debounce
    }
    lastBtn = btn;

    // Knight Rider effect
    for (int i = 0; i < 6; i++) digitalWrite(i, HIGH);
    digitalWrite(pos, LOW);
    delay(speeds[speedIdx]);
    pos += dir;
    if (pos >= 5 || pos <= 0) dir = -dir;
}
