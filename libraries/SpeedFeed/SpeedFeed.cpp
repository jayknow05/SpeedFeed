#include <SpeedFeed.h>
#include <EEPROMAnything.h>

LED::LED(int ledPinRed, int ledPinGrn, int initState) {
  pinRed = ledPinRed;
  pinGrn = ledPinGrn;
  next_state = initState;
  state = initState;
  blink = OFF;
  pinMode(pinRed, OUTPUT);
  pinMode(pinGrn, OUTPUT);
  set(initState);
}

void LED::set(int ledState, int dT) {
  
  highTime = dT;
  startTime = millis();
  if (blink) {
    blink = OFF;
    setPins(OFF);
  }
  
  if (dT == 0) {
    setPins(ledState);
    next_state = ledState;
  }
  else {
    next_state = state;
    setPins(ledState);
  }


}

void LED::blinkContinuous(int color, int high, int low, long timeLimit){
  
  if (blink != color) {
    blink = color;
    lowTime = low;
    highTime = high;
    startTime = millis();
    blinkStartTime = millis();
    endTime = timeLimit;
  
    setPins(color);
    next_state = OFF;
  }
}

void LED::setPins(int ledState) {
  switch(ledState) {

  case RED:
    digitalWrite(pinRed, HIGH);
    digitalWrite(pinGrn, LOW);
    break;
  case GREEN:
    digitalWrite(pinRed, LOW);
    digitalWrite(pinGrn, HIGH);
    break;
  case YELLOW:
    digitalWrite(pinRed, HIGH);
    digitalWrite(pinGrn, HIGH);
    break;
  case OFF:
    digitalWrite(pinRed, LOW);
    digitalWrite(pinGrn, LOW);
    break;
  default:
    Serial.println("Invalid LED State...");
    break;
  }

  state = ledState;

}

Waypoint::Waypoint(double px1, double py1, double px2, double py2, double d) {

 double m = (px1 - px2) / (py2 - py1);
 double b = py2 - m*px2;
 double A = 1 + m*m;
 double B = 2*b*m - 2*px2 - 2*m*py2;
 double C = px2*px2 + b*b - 2*b*py2 + py2*py2 - d*d;

 double det = sqrt(B*B - 4*A*C);

 x1 = (-B + det)/(2*A);
 x2 = (-B - det)/(2*A);

 y1 = (m*x1 + b);
 y2 = (m*x2 + b);

 y1 = floor(y1*100)/100.0;
 y2 = floor(y2*100)/100.0;
 x1 = floor(x1*100)/100.0;
 x2 = floor(x2*100)/100.0;

 split_margin = 0;
 split_best = 0;
 lap_margin = 0;
 split_best = 0;
 adjustment = 0;
 speed = 0;
 speed_best = 0;
 speed_margin = 0;
 last = (long)millis();

};

void reset(std::vector<Waypoint>& myWaypoints) {
  for(std::vector<Waypoint>::iterator it = myWaypoints.begin(); it != myWaypoints.end(); it++) {
    it->speed_margin = 0;
    it->speed_best = 0;
    it->split_margin = 0;
    it->split_best = 0;
    it->lap_margin = 0;
    it->lap_best = 0;
    it->last = 0;
  }
}


void Waypoint::checkSplit(Waypoint obj, long lap_start) {
  
  last = (long)millis() + obj.adjustment;

  long split = last - obj.last;
  long lap = last - lap_start;

  if (split_best == 0) {
    split_best = split;
    lap_best = lap;
  }
  split_margin = split - split_best;
  lap_margin = lap - lap_best;
  speed_margin = obj.speed - speed_best;
  if (split < split_best) {    
    split_best = split;
  }

  if (lap < lap_best) {
    lap_best = lap;
  }

  if (obj.speed > speed_best) {
    speed_best = obj.speed;
  }



}

Point::Point(double px, double py, double pv) {
  x = px;
  y = py;
  t = millis();
  v = pv;
}

void ledUpdate(std::vector<LED>& myLEDs) {
  unsigned long updateTime = millis();
  for(std::vector<LED>::iterator it = myLEDs.begin(); it != myLEDs.end(); it++) {
      if (it->blink) {
        if ((updateTime - it->startTime > it->endTime)&&(it->endTime)) {
          it->blink = OFF;
          it->next_state = OFF;
          it->setPins(it->next_state);
        }
        if (it->state) {
          if (updateTime - it->blinkStartTime > it->highTime) {
            it->setPins(it->next_state);
            it->next_state = it->blink;
            it->blinkStartTime = millis();
          }
        }
        else if (updateTime - it->blinkStartTime > it->lowTime) {
          it->setPins(it->next_state);
          it->next_state = OFF;
          it->blinkStartTime = millis();
        }

      }

      else {
        if (updateTime - it->startTime > it->highTime){
          it->setPins(it->next_state);
        }
      }

  }
}


bool intersection(Point& p1, Point& p2, Waypoint& obj) {
 if (!((p1.x == 0.0)||(p2.x == 0.0))) {
    double x3 = obj.x1;
    double y3 = obj.y1;
    double x4 = obj.x2;
    double y4 = obj.y2;

    double x1 = p1.x;
    double y1 = p1.y;
    double x2 = p2.x;
    double y2 = p2.y;

    double t1 = p1.t;
    double t2 = p2.t;

    double px;
    double py;

    //Serial.print("x1 = ");Serial.print(x1, 8);Serial.print(", y1 = ");Serial.print(y1, 8);
    //Serial.print(", x2 = ");Serial.print(x2, 8);Serial.print(", y2 = ");Serial.println(y2, 8);
    double x1y2_y1x2 = x1*y2-y1*x2;
    //Serial.print("x1y2_y1x2 = ");Serial.println(x1y2_y1x2, 8);
    double x3_x4 = x3-x4;
    //Serial.print("x3 = ");Serial.print(x3, 8);Serial.print(", y3 = ");Serial.print(y3, 8);
    //Serial.print(", x4 = ");Serial.print(x4, 8);Serial.print(", y4 = ");Serial.println(y4, 8);
    //Serial.print("x3_x4 = ");Serial.println(x3_x4, 8);
    double x1_x2 = x1-x2;
   // Serial.print("x1_x2 = ");Serial.println(x1_x2, 8);
    double y3_y4 = y3-y4;
   // Serial.print("y3_y4 = ");Serial.println(y3_y4, 8);
    double y1_y2 = y1-y2;
   // Serial.print("y1_y2 = ");Serial.println(y1_y2, 8);
    double x3y4_y3x4 = x3*y4-y3*x4;
   // Serial.print("x3y4_y3x4 = ");Serial.println(x3y4_y3x4, 8);
    double denom = (x1_x2)*(y3_y4)-(y1_y2)*(x3_x4);
   // Serial.print("denom = ");Serial.println(denom, 8);


    px = ((x1y2_y1x2)*(x3_x4)-(x1_x2)*(x3y4_y3x4)) / denom;
    //Serial.print("px = ");Serial.println(px, 8);
    py = ((x1y2_y1x2)*(y3_y4)-(y1_y2)*(x3y4_y3x4)) / denom;
    //Serial.print("py = ");Serial.println(py, 8);
    if (!(((px <= std::max(x1,x2))&&(px >= std::min(x1,x2)) &&
         (py <= std::max(y3,y4))&&(py >= std::min(y3,y4)))))
    {
   //   Serial.println("FALSE");
      return false;
      
    }
    obj.speed = (p1.v + p2.v)/2;
    obj.adjustment = floor((t1 - t2)*sqrt((x2-px)*(x2-px)+(y2-py)*(y2-py))/sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
   // Serial.println("TRUE");
    return true;

  }

  return false;

}

void storeWaypoints(int deviceaddress, int eeaddress, std::vector<Waypoint>& myWaypoints) {

    EEPROM_writeAnything(deviceaddress, eeaddress++, 0x24);

    EEPROM_writeAnything(deviceaddress, eeaddress++, myWaypoints.size());

    for(std::vector<Waypoint>::iterator it = myWaypoints.begin(); it != myWaypoints.end(); it++) {
      EEPROM_writeAnything(deviceaddress, eeaddress, it->x1);
      eeaddress = eeaddress + 8;
      EEPROM_writeAnything(deviceaddress, eeaddress, it->y1);
      eeaddress = eeaddress + 8;
      EEPROM_writeAnything(deviceaddress, eeaddress, it->x2);
      eeaddress = eeaddress + 8;
      EEPROM_writeAnything(deviceaddress, eeaddress, it->y2);
      eeaddress = eeaddress + 8;
    }
}

void loadWaypoints(int deviceaddress, int eeaddress, std::vector<Waypoint>& myWaypoints) {

  byte c = 0xFF;

  EEPROM_readAnything(deviceaddress, eeaddress++, c);
  Serial.println(c, HEX);
  if (c == 0x24) {
    EEPROM_readAnything(deviceaddress, eeaddress++, c);
    int num = (int)c;

    for (int i = 0; i < num; i++) {
      myWaypoints.push_back(Waypoint(0,0,0,0,0));
      EEPROM_readAnything(deviceaddress, eeaddress, myWaypoints[i].x1);
      eeaddress = eeaddress + 8;
      Serial.print(myWaypoints[i].x1, 8);Serial.print(" ");
      EEPROM_readAnything(deviceaddress, eeaddress, myWaypoints[i].y1);
      eeaddress = eeaddress + 8;
      Serial.print(myWaypoints[i].y1, 8);Serial.print(" ");
      EEPROM_readAnything(deviceaddress, eeaddress, myWaypoints[i].x2);
      eeaddress = eeaddress + 8;
      Serial.print(myWaypoints[i].x2, 8);Serial.print(" ");
      EEPROM_readAnything(deviceaddress, eeaddress, myWaypoints[i].y2);
      eeaddress = eeaddress + 8;
      Serial.println(myWaypoints[i].y2, 8);
    }
  }

}

void sendWaypoints(std::vector<Waypoint>& myWaypoints) {
  
  Serial.print("startfinish");
  Serial.print(",");
  Serial.print(myWaypoints[0].x1, 8);
  Serial.print(",");
  Serial.print(myWaypoints[0].y1, 8);
  Serial.print(",");
  Serial.print(myWaypoints[0].x2, 8);
  Serial.print(",");
  Serial.println(myWaypoints[0].y2, 8);

  for (int i = 1; i < myWaypoints.size(); i++) {  
    
    Serial.print("waypoint");
    Serial.print(",");
    Serial.print(myWaypoints[i].x1, 8);
    Serial.print(",");
    Serial.print(myWaypoints[i].y1, 8);
    Serial.print(",");
    Serial.print(myWaypoints[i].x2, 8);
    Serial.print(",");
    Serial.println(myWaypoints[i].y2, 8);
  
  }
}


