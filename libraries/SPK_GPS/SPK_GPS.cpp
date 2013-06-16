/***********************************
This is our GPS library

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/

#include <SPK_GPS.h>

// how long are max NMEA lines to parse?
#define MAXLINELENGTH 120

// we double buffer: read one line in and leave one for the main program
volatile char line1[MAXLINELENGTH];
volatile char line2[MAXLINELENGTH];
// our index into filling the current line
volatile uint8_t lineidx=0;
// pointers to the double buffers
volatile char *currentline;
volatile char *lastline;
volatile boolean recvdflag;
volatile boolean inStandbyMode;


boolean SPK_GPS::parse(char *nmea) {
  // do checksum check

  // first look if we even have one
  if (nmea[strlen(nmea)-4] == '*') {
    uint16_t sum = parseHex(nmea[strlen(nmea)-3]) * 16;
    sum += parseHex(nmea[strlen(nmea)-2]);
    
    // check checksum 
    for (uint8_t i=1; i < (strlen(nmea)-4); i++) {
      sum ^= nmea[i];
    }
    if (sum != 0) {
      // bad checksum :(
      return false;
    }
  }

  if (strstr(nmea, "$GPGGA")) {
    // found GGA
    char *p = nmea;
    
    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    //uint32_t time = timef;
    //hour = time / 10000;
    //minute = (time % 10000) / 100;
    //seconds = (time % 100);

    milliseconds = (long)(timef * 1000);//fmod(timef, 1.0) * 1000;

    // parse out latitude
    p = strchr(p, ',')+1;
    double deg = floor(atof(p) / 100.0);
    double min = atof(p) - (deg * 100.0);
    latitude = deg*360000.0 + (min*6000.0);

    p = strchr(p, ',')+1;
    if (p[0] == 'N') lat = 'N';
    else if (p[0] == 'S') {
      latitude = 0 - latitude;
      lat = 'S';
    }
    
    // parse out longitude
    p = strchr(p, ',')+1;
    deg = floor(atof(p) / 100.0);
    min = atof(p) - (deg * 100.0);
    longitude = deg*360000.0 + (min*6000.0);

    p = strchr(p, ',')+1;
    if (p[0] == 'W') {
      longitude = 0.0 - longitude;
      lon = 'W';
    }
    else if (p[0] == 'E') lon = 'E';
    else if (p[0] == ',') lon = 0;
    else return false;

    p = strchr(p, ',')+1;
    fixquality = atoi(p);

    p = strchr(p, ',')+1;
    satellites = atoi(p);

    p = strchr(p, ',')+1;
    HDOP = atof(p);

    p = strchr(p, ',')+1;
    altitude = atof(p);
    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;
    geoidheight = atof(p);
    return true;
  }

  if (strstr(nmea, "$GPRMC")) {
   // found RMC
    char *p = nmea;
    lat_old = latitude;
    long_old = longitude;
    
    // get time
    p = strchr(p, ',')+1;
    double timef = atof(p);

    milliseconds = timef * 1000;//fmod(timef, 1.0) * 1000;

    p = strchr(p, ',')+1;
    // Serial.println(p);
    if (p[0] == 'A') 
      fix = true;
    else if (p[0] == 'V')
      fix = false;
    else
      return false;

    // parse out latitude
    p = strchr(p, ',')+1;
    double deg = floor(atof(p) / 100.0);
    double min = atof(p) - (deg * 100.0);
    latitude = deg*360000.0 + (min*6000.0);

    p = strchr(p, ',')+1;
    if (p[0] == 'N') lat = 'N';
    else if (p[0] == 'S') {
      latitude = 0 - latitude;
      lat = 'S';
    }
    else if (p[0] == ',') lat = 0;
    else return false;

    // parse out longitude
    p = strchr(p, ',')+1;
    deg = floor(atof(p) / 100.0);
    min = atof(p) - (deg * 100.0);
    longitude = deg*360000.0 + (min*6000.0);

    p = strchr(p, ',')+1;
    if (p[0] == 'W') {
      longitude = 0.0 - longitude;
      lon = 'W';
    }
    else if (p[0] == 'E') lon = 'E';
    else if (p[0] == ',') lon = 0;
    else return false;

    // speed
    p = strchr(p, ',')+1;
    speed = atof(p);

    // angle
    p = strchr(p, ',')+1;
    angle = atof(p);

    p = strchr(p, ',')+1;
    uint32_t fulldate = atof(p);
    day = fulldate / 10000;
    month = (fulldate % 10000) / 100;
    year = (fulldate % 100);

    // we dont parse the remaining, yet!
    return true;
  }

  return false;
}

char SPK_GPS::read(void) {
  char c = 0;
  
  if (paused) return c;

  /*if(gpsSwSerial) {
    if(!gpsSwSerial->available()) return c;
    c = gpsSwSerial->read();
  } else {
    if(!gpsHwSerial->available()) return c;
    c = gpsHwSerial->read();
  }*/
  if(!gpsHwSerial->available()) return c;
  c = gpsHwSerial->read();
  //Serial.print(c);

  if (c == '$') {
    currentline[lineidx] = 0;
    lineidx = 0;
  }
  
  if (c == '\n') {
    currentline[lineidx] = 0;
    if (currentline == line1) {
      currentline = line2;
      lastline = line1;
    } else {
      currentline = line1;
      lastline = line2;
    }

    //Serial.println("----");
    //Serial.println((char *)lastline);
    //Serial.println("----");
    lineidx = 0;
    recvdflag = true;
  }
  
  if ((c != '\r')&(c != '\n')&(c != '\r\n')) {
    currentline[lineidx++] = c;
  }
  
  if (lineidx >= MAXLINELENGTH)
    lineidx = MAXLINELENGTH-1;

  return c;
}

// Constructor when using SoftwareSerial or NewSoftSerial
/*#if ARDUINO >= 100
SPK_GPS::SPK_GPS(SoftwareSerial *ser)
#else
SPK_GPS::SPK_GPS(NewSoftSerial *ser) 
#endif
{
  common_init();     // Set everything to common state, then...
  gpsSwSerial = ser; // ...override gpsSwSerial with value passed.
}*/

// Constructor when using HardwareSerial
SPK_GPS::SPK_GPS(HardwareSerial *ser) {
  common_init();  // Set everything to common state, then...
  gpsHwSerial = ser; // ...override gpsHwSerial with value passed.
}

// Initialization code used by all constructor types
void SPK_GPS::common_init(void) {
  //gpsSwSerial = NULL; // Set both to NULL, then override correct
  gpsHwSerial = NULL; // port pointer in corresponding constructor
  recvdflag   = false;
  paused      = false;
  lineidx     = 0;
  currentline = line1;
  lastline    = line2;

  hour = minute = seconds = year = month = day =
    fixquality = satellites = 0; // uint8_t
  lat = lon = mag = 0; // char
  fix = false; // boolean
  milliseconds = 0; // uint16_t
  latitude = longitude = geoidheight = altitude =
    speed = angle = magvariation = HDOP = 0.0; // float
}

void SPK_GPS::begin(uint16_t baud)
{
  /*if(gpsSwSerial) gpsSwSerial->begin(baud);
  else */           
  gpsHwSerial->begin(baud);

  delay(10);
}

void SPK_GPS::sendCommand(char *str) {
  /*if(gpsSwSerial) gpsSwSerial->println(str);
  else    */        
  gpsHwSerial->println(str);
}

boolean SPK_GPS::newNMEAreceived(void) {
  return recvdflag;
}

void SPK_GPS::pause(boolean p) {
  paused = p;
}

char *SPK_GPS::lastNMEA(void) {
  recvdflag = false;
  return (char *)lastline;
}

// read a Hex value and return the decimal equivalent
uint8_t SPK_GPS::parseHex(char c) {
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
}

boolean SPK_GPS::waitForSentence(char *wait4me, uint8_t max) {
  char str[20];

  uint8_t i=0;
  while (i < max) {
    if (newNMEAreceived()) { 
      char *nmea = lastNMEA();
      strncpy(str, nmea, 20);
      str[19] = 0;
      i++;

      if (strstr(str, wait4me))
	return true;
    }
  }

  return false;
}

// Standby Mode Switches
boolean SPK_GPS::standby(void) {
  if (inStandbyMode) {
    return false;  // Returns false if already in standby mode, so that you do not wake it up by sending commands to GPS
  }
  else {
    inStandbyMode = true;
    sendCommand(PMTK_STANDBY);
    //return waitForSentence(PMTK_STANDBY_SUCCESS);  // don't seem to be fast enough to catch the message, or something else just is not working
    return true;
  }
}

boolean SPK_GPS::wakeup(void) {
  if (inStandbyMode) {
   inStandbyMode = false;
    sendCommand("");  // send byte to wake it up
    return waitForSentence(PMTK_AWAKE);
  }
  else {
      return false;  // Returns false if not in standby mode, nothing to wakeup
  }
}