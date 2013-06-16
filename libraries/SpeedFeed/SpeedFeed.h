#ifndef _SPEED_FEED_H
#define _SPEED_FEED_H

#include "Arduino.h"
#include <vector>

#define OFF     0
#define RED     1
#define GREEN   2
#define YELLOW  3

class LED { 

  public:
  int pinRed;
  int pinGrn;
  int state;
  int blink;
  int next_state;
  int highTime;
  int lowTime;
  int startTime;
  long endTime;
  long blinkStartTime;
  LED(int ledPinRed, int ledPinGrn, int initState);
  void set(int ledState, int dT = 0);
  void blinkContinuous(int color, int high, int low, long timeLimit = 0);
  void setPins(int ledState);
};

void ledUpdate(std::vector<LED>& myLEDs);

class Waypoint {
  
  public:
  double x1;
  double y1;
  double x2;
  double y2;
  long split_best;
  long split_margin;
  long last;
  long lap_best;
  long lap_margin;
  double speed;
  double speed_best;
  double speed_margin;
  long adjustment;
  Waypoint(double px1, double py1, double px2, double py2, double d);
  void checkSplit(Waypoint obj, long lap_start);

};

void reset(std::vector<Waypoint>& myWaypoints);

class Point {

  public:
  double x;
  double y;
  double v;
  long t;
  Point(double px, double py, double pv);

};

bool intersection(Point& p1, Point& p2, Waypoint& obj);
void storeWaypoints(int deviceaddress, int eeaddress, std::vector<Waypoint>& myWaypoints);
void loadWaypoints(int deviceaddress, int eeaddress, std::vector<Waypoint>& myWaypoints);
void sendWaypoints(std::vector<Waypoint>& myWaypoints);

#endif