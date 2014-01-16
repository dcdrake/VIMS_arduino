void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int rawPressureInput = analogRead(A1);
  
  float PSI = rawPressureInput * (.2901);
  
  Serial.println(PSI);
  //Serial.println(rawPressureInput);
  delay(100);
}
