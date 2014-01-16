void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);
}

// the loop routine runs over and over again forever:
void loop() {
  int rawFluoroInput = analogRead(A0);
  int rawPressureInput = analogRead(A1);
  
  
  // convert the number of counts to chlorophyll conc:
  float CHL = 0.0125 * (rawFluoroInput - 48);
  //convert the raw analog data into PSI:
  float PSI = rawPressureInput * (.2901);
  
  Serial.print("The chlorophyll concentration is: ");
  Serial.println(CHL);
  Serial.print("The pressure is: ");
  Serial.println(PSI);
  delay(300);
}
