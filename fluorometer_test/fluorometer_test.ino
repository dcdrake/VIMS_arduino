void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int rawFluoroInput = analogRead(A0);
  
  // convert the number of counts to chlorophyll conc.
  float CHL = 0.0125 * (rawFluoroInput - 48);
  
  Serial.print("The chlorophyll concentration is: ");
  Serial.println(CHL);
  delay(100);
}
