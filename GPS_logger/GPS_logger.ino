#include <SPK_GPS.h>
#include <SPI.h>
#include <SD.h>
#include <SpeedFeed.h>
#include <vector>
#include <EEPROMAnything.h>
#include <Wire.h>

std::vector<LED> myLEDs;
std::vector<Point> myTracks;

SPK_GPS GPS(&Serial3);

File logfile;

#define ST_RESET  0
#define ST_INIT   1
#define ST_RACE   2

#define INVALID   0
#define GPS_FIX   1
#define DGPS_FIX  2

#define LED_LAP      0
#define LED_SPLT     1

#define PIN_PB       3
#define PIN_LAP_RED  8                                                                                      
#define PIN_LAP_GRN  6
#define PIN_SPLT_RED 12
#define PIN_SPLT_GRN 11

const int chipSelect = 4;

int pbFlag = 0, wp_next = 0, wp_last, max_tracksize = 4, current_state = ST_RESET, next_state = ST_RESET;

void interruptPB(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 400) 
  {
    pbFlag = 1;
  }
  last_interrupt_time = interrupt_time; 
}

void setup() {

  while (!Serial) ;

  Serial.begin(9600);
  Serial.println("\n"); // clear out garbage on the serial line...
  Serial.println("reset,");
  Serial.println("reset,");
  Serial3.begin(9600);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    //if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
  }
  char filename[15];
  strcpy(filename, "GPSLOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    return;
  }
  Serial.print("Writing to "); 
  Serial.println(filename);


  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY); 
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PMTK_ENABLE_SBAS);
  GPS.sendCommand(PMTK_API_SET_DGPS_MODE_WAAS);
  GPS.sendCommand(PMTK_Q_EASY);
  pinMode(PIN_PB,INPUT_PULLUP);

  myLEDs.push_back(LED(PIN_LAP_RED, PIN_LAP_GRN, OFF));
  myLEDs.push_back(LED(PIN_SPLT_RED, PIN_SPLT_GRN, OFF));
  attachInterrupt(PIN_PB, interruptPB,  FALLING);

}

void loop() {

  char c = GPS.read();
  //if (c) Serial.print(c);

  switch(current_state) {

  case ST_RESET:
    static int bootFlag = 0;
    if (!bootFlag) {
      if (!digitalRead(PIN_PB)) {
        while (!digitalRead(PIN_PB));
        myLEDs[LED_SPLT].set(YELLOW);
        myLEDs[LED_LAP].set(YELLOW);
        Serial.println("Started in Service Mode");
        pbFlag = 0;
        wp_next = 0;
        next_state = ST_RACE;
        current_state = next_state;
        bootFlag = 1;
        break;
      }
      bootFlag = 1;
    }
    switch (GPS.fixquality) {
    case INVALID:
      myLEDs[LED_LAP].blinkContinuous(RED, 200, 800);
      myLEDs[LED_SPLT].blinkContinuous(RED, 200, 800);
      break;
    case GPS_FIX:
      myLEDs[LED_LAP].blinkContinuous(YELLOW, 500, 500);
      myLEDs[LED_SPLT].blinkContinuous(YELLOW, 500, 500);
      if (pbFlag) {
        pbFlag = 0;
        next_state = ST_RACE;
        GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
        myLEDs[LED_LAP].blinkContinuous(GREEN, 500, 500);
        myLEDs[LED_SPLT].blinkContinuous(GREEN, 500, 500);
      }
      break;    
    case DGPS_FIX:
      next_state = ST_RACE;
      GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
      myLEDs[LED_LAP].blinkContinuous(GREEN, 500, 500);
      myLEDs[LED_SPLT].blinkContinuous(GREEN, 500, 500);
      break;
    default:
      break;
    }
    break;

  case ST_RACE:
    break;
  }

  if (GPS.newNMEAreceived()) {
    current_state = next_state;
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
    char *stringptr = GPS.lastNMEA();
    uint8_t stringsize = strlen(stringptr);
    if (stringsize != logfile.write((uint8_t *)stringptr, stringsize))    //write the string to the SD file
    if (strstr(stringptr, "RMC"))   logfile.flush();
  }
  ledUpdate(myLEDs);
  current_state = next_state;
}


























