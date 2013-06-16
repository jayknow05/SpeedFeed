/*####################################################################
 FILE: autoCalibration.ino
 VERSION: 1S0B
 PURPOSE: Shows how to use automatic calibration and data output
 LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)

 GET UPDATES: https://www.virtuabotix.com/

 HISTORY:
 Joseph Dattilo (Virtuabotix LLC) - Version 1S0B (09/20/12)
 -Fixed bug in autoCalibration.ino where Serial.begin was not called.
 Joseph Dattilo (Virtuabotix LLC) - Version 1S0A (07/26/12)
#######################################################################*/

#include "Accelerometer.h"

Accelerometer myAccelerometer = Accelerometer();

void setup()
{
  Serial.begin(9600);

  //Connect up the following pins and your power rail
  //                    SL GS 0G   X  Y  Z
  myAccelerometer.begin(3, 4, 5, A0, A1, A2);

//calibration performed below
Serial.println("Please place the Accelerometer on a flat\nLevel surface");
delay(2000);//Give user 2 seconds to comply
myAccelerometer.calibrate();
}

void loop()
{
  delay(2000);//delay for readability
  //reads the values of your accelerometer
  myAccelerometer.read();
  Serial.println("The Accelerometer reads X, Y, Z");
  Serial.print(myAccelerometer._Xgs);
  Serial.print(" Gs, ");
  Serial.print(myAccelerometer._Ygs);
  Serial.print(" Gs, and ");
  Serial.print(myAccelerometer._Zgs);
  Serial.println(" Gs respectively.");
}
