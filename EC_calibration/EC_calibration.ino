/*
EC slope 1.90% / deg C
EC25 = temp compensate to 25C
EC25 = ECf / (1 + x(t - 25))
 
EC18 = temp compensate to 18C
EC18 = ECf / (1+ x(t - 18))
 
ECf = ec value
x = coefficent (1.9%)
t = temperature in C
 
A couple notes:
- It seems like the standard way to measure EC is to temp correct to 15C, although I&#39;m temp correcting to 25C
- Lots of info here: <a href="http://reefcentral.com/forums/showthread.php?s=&amp;threadid=914676&amp;highlight=sigma
-">http://reefcentral.com/forums/showthread.php?s=&amp;threadid=914676&amp;...</a> there has been some research done that indicates that x (formulas above) is not constant, but changes with temperature
- this sketch can probably be improved upon. let me know if there are any mistakes
 
About what this sketch is doing.
 
When you run it to configure it you&#39;ll need 2 EC solutions. 53mS and 80mS are preferred
 
First you place your probe into the 53mS solution which changes the frequency that the 555 generates. Its recommended to leave
it in solution for 10 - 20 min for readings to stabilize
 
That will give you once frequency reading and one calibration value
 
ie&gt; 19424hz and 53mS
Change Y1 and X1
#define Y1 19424
#define X1 53
 
Next you place the probe in 80mS and repeat which will give you your second reading
 
ie&gt; 29421hz and 80mS
 
#define Y2 29421
#define X2 80
 
Once you have those values the script basically figures out the slope of your line. When you place it in solution
the frequence will fall on the slope and can thus be figured out
 
After the value has been determined temperature compensation can be applied. In my opinion this still needs a lot more research
For most aquariusts the effects will be negligible (especially if you calibrate at the same temp your aquarium is), but there&#39;s
a lot of research that the x (compensation factor) in the formula isn&#39;t a constant, but varies from 1% to 3% from 3C to 35C
*/
 
#define SAMPLES 4096
 
#define Y1 0
#define Y2 0
#define X1 0
#define X2 0
 
int enable_pin = 4;
int reading_pin = 7;
unsigned long freqhigh = 0;
unsigned long freqlow = 0;
unsigned long frequency = 0;
 
void setup() {
  pinMode(reading_pin, INPUT);
  pinMode(enable_pin, OUTPUT);
  Serial.begin(9600);
}
 
 
void loop() {
digitalWrite(4, HIGH);
#define SAMPLES 4096
unsigned long freqhigh = 0;
unsigned long freqlow = 0;
unsigned long frequency = 0;
for(unsigned int j=0; j<SAMPLES; j++) {
  freqhigh+= pulseIn(7, HIGH, 250000);
  freqlow+= pulseIn(7, LOW, 250000);
}
/*Serial.print("freqhigh: ");
Serial.println(freqhigh);
Serial.print("freqlow: ");
Serial.println(freqlow);
//Serial.println(freq / SAMPLES);  
Serial.print("High: ");
Serial.println(freqhigh / SAMPLES);
Serial.print("Low: ");
Serial.println(freqlow / SAMPLES);*/
 
frequency = 1000000 / ( (freqhigh / SAMPLES) + (freqlow / SAMPLES) );
 
Serial.print("Frequency: ");
Serial.println(frequency);
 
//int val = map(frequency, 1006, 8333, 1061, 52631);
 
// y = mx + b
 
long m = (8333 - 52631) / (1.006 - 1.061);
 
long b = 8333 - (m * 1.006);
 
float x = ((float)frequency - (float)b) / (float)m;
 
/*Serial.print("m= ");
Serial.println(m);
Serial.print("b= ");
Serial.println(b);*/
Serial.print("Salinity: ");
Serial.println(x, 4);
 
  digitalWrite(4, LOW);  
  delay(30000);
}
