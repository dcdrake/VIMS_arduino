#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
const int LOG_INTERVAL = 1000; // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
const int SYNC_INTERVAL = 1000; // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL  1 // echo data to serial port
#define WAIT_TO_START  0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
const int redLEDpin = 2;
const int greenLEDpin = 3;
const int onSwitch = 4;
//const int switchStatusLED = 13;

const int pressurePin = 0;           // analog 0 
const int fluoroPin = 1;             // analog 1

/*const int BANDGAPREF = 14;

//const float aref_voltage = 5.0;         // we tie 3.3V to ARef and measure it with a multimeter!
const float bandgap_voltage = 1.1;      // this is not super guaranteed but its not -too- off
*/

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  //int switchState;
  
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  //pinMode(switchStatusLED, OUTPUT);
  pinMode(onSwitch, INPUT);
  
#if WAIT_TO_START
  Serial.println("Wave magnet to start");
  do {
  switchState = digitalRead(onSwitch);
  Serial.println(switchState);
  } while (switchState == 0);
#endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
  

  logfile.println("stamp,datetime,pressure,fluoro");    
#if ECHO_TO_SERIAL
  Serial.println("stamp,datetime,pressure,fluoro");
#endif //ECHO_TO_SERIAL
 
  // If you want to set the aref to something other than 5v
  //analogReference(EXTERNAL);
}

void loop(void)
{
  int switchState = digitalRead(onSwitch);
  //if switch is high, log data from sensors
  if(switchState == 1){
  DateTime now;
  
  digitalWrite(greenLEDpin, HIGH);

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.unixtime()); // seconds since 1/1/1970
  logfile.print(", ");
  logfile.print('"');
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL 

  int pressureReading = analogRead(pressurePin);
  float PSI = pressureReading;// * (.2901);  
   
  
  logfile.print(", ");    
  logfile.print(PSI);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(PSI);
#endif //ECHO_TO_SERIAL

int fluoroReading = analogRead(fluoroPin);
float CHL = 0.0125 * (fluoroReading - 48);//correct 48 dark counts, 48 * 4133/1024

  logfile.print(", ");    
  logfile.print(CHL);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(CHL);
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

  digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  digitalWrite(redLEDpin, LOW);
  delay(1000);
  }
}
