/*Pinos usados:
  D9 - Load Pin do 74HC165
  D12 - Q7 Pin do 74HC165
  D13 - CLOCK Pin do 74HC165

  Sketch que usa o shiftIn.
https://jpralves.net/post/2015/09/08/multiple-inputs-parallel-to-series.html
https://playground.arduino.cc/Code/ShiftRegSN74HC165N/
https://www.ti.com/lit/ds/symlink/sn74hc165.pdf?ts=1649753640730&ref_url=https%253A%252F%252Fwww.google.com%252F
*/

const byte LATCH = D4;//9;
const byte DATA = D2;//12;
const byte CLOCK = D0;//13;

void setup() {
  Serial.begin(115200);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, INPUT);

  digitalWrite(CLOCK, HIGH);
  digitalWrite(LATCH, HIGH);
  Serial.print("stuo : ");
}

void loop() {
  Serial.print("loop : ");
  digitalWrite(CLOCK, HIGH);
  digitalWrite(LATCH, LOW);
  delayMicroseconds (5);
  digitalWrite(LATCH, HIGH);
  delayMicroseconds (5);

  byte optionSwitch = shiftIn(DATA, CLOCK, MSBFIRST);

  Serial.print("12345678 : ");
  for (byte i=0; i<8; i++) {
    Serial.print((optionSwitch & (1<<i)) != 0 ? 1 : 0);
  }
  Serial.println();

  delay(100);
}
// Sketch uses 3,048 bytes (9%) of program storage space. Maximum is 32,256 bytes.
// Global variables use 212 bytes (10%) of dynamic memory, leaving 1,836 bytes for local variables. Maximum is 2,048 bytes.

// Read more at: https://jpralves.net/post/2015/09/08/multiple-inputs-parallel-to-series.html
