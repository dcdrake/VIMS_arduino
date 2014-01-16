int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (5.0 / 1023.0);  
  //float tempCelsius = (-241.54 * voltage) + 586.74;
  Serial.println(sensorValue);
  delay(100);  
}

