#include <SPK_GPS.h>
#include <SPI.h>
#include <SD.h>
#include <SpeedFeed.h>
#include <vector>
#include <EEPROMAnything.h>
#include <Wire.h>

#define disk1 0x50

std::vector<Waypoint> myWaypoints;
std::vector<LED> myLEDs;
std::vector<Point> myTracks;

SPK_GPS GPS(&Serial3);

#define ST_RESET  0
#define ST_INIT   1
#define ST_SFSET  2
#define ST_WPSET  3
#define ST_RACE   4

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

  Wire.begin();

  Serial.begin(9600);
  Serial.println("\n"); // clear out garbage on the serial line...
  Serial.println("reset,");
  Serial.println("reset,");
  Serial3.begin(9600);

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
        next_state = ST_WPSET;
        current_state = next_state;
        bootFlag = 1;
        break;
      }
      loadWaypoints(disk1, 0, myWaypoints);
      sendWaypoints(myWaypoints);  
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
        next_state = ST_INIT;
        GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
        myLEDs[LED_LAP].blinkContinuous(GREEN, 500, 500);
        myLEDs[LED_SPLT].blinkContinuous(GREEN, 500, 500);
      }
      break;    
    case DGPS_FIX:
      next_state = ST_INIT;
      GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
      myLEDs[LED_LAP].blinkContinuous(GREEN, 500, 500);
      myLEDs[LED_SPLT].blinkContinuous(GREEN, 500, 500);
      break;
    default:
      break;
    }
    break;

  case ST_INIT:
    break;

  case ST_WPSET:
    if (pbFlag == 1) {
      if ((myTracks.size() >= 2)&&(GPS.fixquality)) {
        myWaypoints.push_back(Waypoint(myTracks[myTracks.size() - 2].x, myTracks[myTracks.size() - 2].y, myTracks[myTracks.size() - 1].x, myTracks[myTracks.size() - 1].y, 72));
        if (!(isnan(myWaypoints[myWaypoints.size() - 1].x1)|| isnan(myWaypoints[myWaypoints.size() - 1].y1) || isnan(myWaypoints[myWaypoints.size() - 1].x2) || isnan(myWaypoints[myWaypoints.size() -1 ].y2))) {
          if (myWaypoints.size() == 1) Serial.print("startfinish");
          /*else Serial.print("waypoint");
           Serial.print(",");
           Serial.print(myWaypoints[myWaypoints.size()-1].x1, 8);
           Serial.print(",");
           Serial.print(myWaypoints[myWaypoints.size()-1].y1, 8);
           Serial.print(",");
           Serial.print(myWaypoints[myWaypoints.size()-1].x2, 8);
           Serial.print(",");
           Serial.println(myWaypoints[myWaypoints.size()-1].y2, 8);*/
          myTracks.clear();
          myLEDs[LED_SPLT].set(GREEN, 3000);
          myLEDs[LED_LAP].set(GREEN, 3000);
        }
        else {
          myWaypoints.pop_back();
          myLEDs[LED_SPLT].set(RED, 1000);
          myLEDs[LED_LAP].set(RED, 1000);
        }
      }
      else {
        myLEDs[LED_SPLT].set(RED, 1000);
        myLEDs[LED_LAP].set(RED, 1000);
      }
      pbFlag = 0;
    }
    break;

  case ST_RACE:
    static int resetFlag = 0;
    static unsigned long pbTimer = millis();
    if (pbFlag) {
      resetFlag = 1;
      pbFlag = 0;
      pbTimer = millis();
    }
    if (!digitalRead(PIN_PB) && resetFlag) {
      if (millis() - pbTimer > 2000) {
        reset(myWaypoints);
        resetFlag = 0;
      }
    }
    break;       
  }

  if (GPS.newNMEAreceived()) {
    current_state = next_state;
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
    Serial.println(GPS.lastNMEA());
    if (GPS.fix) {

      /*Serial.print("track");
       Serial.print(",");
       Serial.print(GPS.longitude, 8);
       Serial.print(",");
       Serial.println(GPS.latitude, 8);*/

      myTracks.push_back(Point(GPS.longitude,GPS.latitude,GPS.speed));
      if (myTracks.size() >= max_tracksize) myTracks.erase(myTracks.begin());

      /*Serial.print("test");
       Serial.print(",");
       Serial.print(myTracks[0].x, 8);
       Serial.print(",");
       Serial.print(myTracks[0].y, 8);
       Serial.print(",");
       Serial.print(myTracks[myTracks.size() - 1].x, 8);
       Serial.print(",");
       Serial.println(myTracks[myTracks.size() - 1].y, 8);*/
      switch(current_state) {

      case ST_RESET:
        break;
      case ST_INIT:
        if (intersection(myTracks[0],myTracks[myTracks.size() - 1], myWaypoints[0])){
          myTracks.clear();
          myWaypoints[0].last = (long)millis() + myWaypoints[0].adjustment;
          myLEDs[LED_LAP].set(OFF);
          myLEDs[LED_SPLT].set(OFF);
          next_state = ST_RACE;
          wp_last = 0;
          wp_next = 1;
          if (wp_next >= myWaypoints.size()) wp_next = 0;
        }
        break;

      case ST_WPSET:        
        if (intersection(myTracks[0],myTracks[myTracks.size() - 1], myWaypoints[0])){
          myTracks.clear();
          myWaypoints[wp_next].checkSplit(myWaypoints[wp_last], myWaypoints[0].last);
          myLEDs[LED_LAP].set(OFF);
          myLEDs[LED_SPLT].set(OFF);          
          storeWaypoints(disk1, 0, myWaypoints);
          next_state = ST_RACE;
          wp_last = 0;
          wp_next = 1;
          if (wp_next >= myWaypoints.size()) wp_next = 0;
        }

        break;
      case ST_RACE:
        static unsigned long resetTimer = millis();      
        if (intersection(myTracks[0],myTracks[myTracks.size() - 1], myWaypoints[wp_next])){
          myWaypoints[wp_next].checkSplit(myWaypoints[wp_last], myWaypoints[0].last);
          /*Serial.println(myWaypoints[wp_next].adjustment);
           Serial.print("split");
           Serial.print(",");
           Serial.print(myWaypoints[wp_next].lap_margin);
           Serial.print(",");
           Serial.print(myWaypoints[wp_next].lap_best);
           Serial.print(",");
           Serial.print(myTracks[myTracks.size() - 1].x, 8);
           Serial.print(",");
           Serial.print(myTracks[myTracks.size() - 1].y, 8);
           Serial.print(",");
           Serial.print(myTracks[0].x, 8);
           Serial.print(",");
           Serial.println(myTracks[0].y, 8);*/
          myTracks.clear();

          int blinkrate = 500;
          if (myWaypoints[wp_next].split_margin < -200) {
            if (myWaypoints[wp_next].split_margin < -1000) blinkrate = 100;
            else if (myWaypoints[wp_next].split_margin < -800) blinkrate = 200;
            else if (myWaypoints[wp_next].split_margin < -400) blinkrate = 300;
            else if (myWaypoints[wp_next].split_margin < -300) blinkrate = 400;
            myLEDs[LED_LAP].set(OFF);
            myLEDs[LED_SPLT].set(OFF);
            myLEDs[LED_SPLT].blinkContinuous(GREEN, blinkrate, blinkrate, 5000);
            myLEDs[LED_LAP].blinkContinuous(GREEN, blinkrate, blinkrate, 5000);
          }

          else if (myWaypoints[wp_next].split_margin > 200) {
            if (myWaypoints[wp_next].split_margin > 1000) blinkrate = 100;
            else if (myWaypoints[wp_next].split_margin > 800) blinkrate = 200;
            else if (myWaypoints[wp_next].split_margin > 400) blinkrate = 300;
            else if (myWaypoints[wp_next].split_margin > 300) blinkrate = 400;   
            myLEDs[LED_LAP].set(OFF);
            myLEDs[LED_SPLT].set(OFF);
            myLEDs[LED_SPLT].blinkContinuous(RED, blinkrate, blinkrate, 5000);
            myLEDs[LED_LAP].blinkContinuous(RED, blinkrate, blinkrate, 5000);
          }

          else {
            myLEDs[LED_LAP].set(OFF);
            myLEDs[LED_SPLT].set(OFF);         
          }

          wp_last = wp_next;
          wp_next += 1;
          if (wp_next >= myWaypoints.size()) wp_next = 0;
        }
        else if (GPS.speed > 1) {
          resetTimer = millis();
        }
        else if ((millis() - resetTimer > 1000) && resetTimer) {
          reset(myWaypoints);
          next_state = ST_INIT;
          myLEDs[LED_LAP].blinkContinuous(GREEN, 500, 500);
          myLEDs[LED_SPLT].blinkContinuous(GREEN, 500, 500);
        }

        break;
      }
    }
  }
  ledUpdate(myLEDs);
  current_state = next_state;
}

























