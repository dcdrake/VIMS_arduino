/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 Modifed by Dylan Drake
 August 2012
 
 */
//CONDUCTIVITY VARIABLES
#define SAMPLES 4096
 
#define Y1 0
#define Y2 0
#define X1 0
#define X2 0
 
int enable_pin = 4;
int reading_pin = 7;
/*unsigned long freqhigh = 0;
unsigned long freqlow = 0;
unsigned long frequency = 0;
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x1A, 0xA7 };
IPAddress ip(139,70,28,238);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

//THERMISTOR VARIABLES
// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000   

int samples[NUMSAMPLES];

//AMT PROBE variables

/*Delete this line to uncomment

//Which analog pin for pH probe
#define pHPIN A1

//Which analog pin for O2 probe
#define O2PIN A2

//Which analog pin for light sensor
//#define LIGHTPIN A3

delete this line to uncomment*/

void setup() {
  pinMode(reading_pin, INPUT);
  pinMode(enable_pin, OUTPUT);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  //CONNECTION TO INTERNET BEGIN
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          
          //CONNECTION TO INTERNET COMPLETE
          
          //define vars for temperature calcualation
          uint8_t i;
          float average;
          
 //FINDING TEMPERATURE 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
 //Is this delay needed?
 delay(1000);

//FINDING SALINITY 
digitalWrite(4, HIGH);
#define SAMPLES 4096
unsigned long freqhigh = 0;
unsigned long freqlow = 0;
unsigned long frequency = 0;
for(unsigned int j=0; j<SAMPLES; j++) {
  freqhigh+= pulseIn(7, HIGH, 250000);
  freqlow+= pulseIn(7, LOW, 250000);
}
 
frequency = 1000000 / ( (freqhigh / SAMPLES) + (freqlow / SAMPLES) );
 
long m = (8333 - 52631) / (1.006 - 1.061);
 
long b = 8333 - (m * 1.006);
 
float x = ((float)frequency - (float)b) / (float)m;
 
  digitalWrite(4, LOW);
 
 //This delay should be 30000 ms. Change to 15000 if testing.
  delay(1000);


      //PRINT OUT DATA TO WEB CLIENT
            client.print("VIMS AUV lab temperature is "); 
            client.print(steinhart);
            client.println(" *C");
            client.println("<br />"); 
            client.print("Frequency: ");
            //client.print(frequency);          Commenting the next six lines
            //                                  appears to make the program run 
            //                                  much faster. Time complexity 
            //                                  analysis of client.print() command needed                             
            client.println("<br />");
            client.print("Salinity: ");
            //client.print(x, 4);       
            client.println("<br />"); 
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
