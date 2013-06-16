/*####################################################################
 FILE: Accelerometer.cpp
 VERSION: 1S1A
 PURPOSE: Runs MMA7361
 LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)

 GET UPDATES: https://www.virtuabotix.com/

 HISTORY:
 Joseph Dattilo (Virtuabotix LLC) - Version 1S1A (11/19/12)
 -Added multiple functions for Operation on Versalino Sense & Move
 Joseph Dattilo (Virtuabotix LLC) - Version 1S0B (09/20/12)
 -Fixed bug in autoCalibration.ino where Serial.begin was not called.
 Joseph Dattilo (Virtuabotix LLC) - Version 1S0A (07/26/12)
#######################################################################*/

#ifndef ACCELEROMETER_H

#include "Accelerometer.h"

//############ Your Library Functions Here #############

int Accelerometer::readAverage(int pin, int samples)
{
long readings = 0;

for(int i = 0; i < samples; i++)
{
readings += analogRead(pin);
}

return (readings/samples);
}


float Accelerometer::convert(float myReading, float my0G)
{
return convert(myReading, my0G, GFORCE);//defaults to G-Forces
}

float Accelerometer::convert(float myReading, float my0G, bool myUnits)
{
float myResult=((my0G-myReading)/(_gc));
if(myUnits)
return myResult; //G-Forces
else
return (myResult * 9.80665);
}

void Accelerometer::calibrate()
{
	_0z = readAverage(_zpin, 10) - _gc;
	_0x = readAverage(_xpin, 10);
	_0y = readAverage(_ypin, 10);

	_calibrated = true;
}

void Accelerometer::read()
{
	_Zgs = convert(analogRead(_zpin), _0z);
	_Ygs = convert(analogRead(_ypin), _0y);
	_Xgs = convert(analogRead(_xpin), _0x);
}

/*void Accelerometer::read()
{
	read(1);
}*/

/*void Accelerometer::read(uint8_t numberOfReads)
{
	_Zraw = readAverage(_zpin, numberOfReads);
	_Yraw = readAverage(_ypin, numberOfReads);
	_Xraw = readAverage(_xpin, numberOfReads);
	_Zgs = convert(_Zraw, _0z);
	_Ygs = convert(_Yraw, _0y);
	_Xgs = convert(_Xraw, _0x);
}*/



//#####################################################


//######### Initialization and Pin Structures ##########

Accelerometer::Accelerometer()
{
}

Accelerometer::Accelerometer(VersalinoBUS myBus)
{
	setBUS(myBus); //assumes you are using the Sense & Move Loadboard on the Versalino
}

Accelerometer::Accelerometer(uint8_t mySL, uint8_t myGS, uint8_t my0G, uint8_t myXPIN, uint8_t myYPIN, uint8_t myZPIN)
{
	begin(mySL, myGS, my0G, myXPIN, myYPIN, myZPIN);
}


void Accelerometer::begin(VersalinoBUS myBus)
{
	setBUS(myBus);
}

void Accelerometer::setBUS(VersalinoBUS myBus)
{
	begin(NOPIN, NOPIN, myBus.D1, myBus.AN0, myBus.AN1, myBus.AN2);
}

void Accelerometer::begin(uint8_t mySL, uint8_t myGS, uint8_t my0G, uint8_t myXPIN, uint8_t myYPIN, uint8_t myZPIN)
{
	_zpin = myZPIN;
	_ypin = myYPIN;
	_xpin = myXPIN;
	_0G = my0G;
	_SL = mySL;
	_GS = myGS;

	if(_SL !=NOPIN)
	pinMode(_SL,OUTPUT);//output mode
	if(_GS !=NOPIN)
	pinMode(_GS,OUTPUT);//output mode
	pinMode(_xpin, INPUT);//input mode
	pinMode(_ypin, INPUT);//input mode
	pinMode(_zpin, INPUT);//input mode

	if(_GS !=NOPIN)
	digitalWrite(_GS,LOW);//sets GS mode
	if(_SL !=NOPIN)
	digitalWrite(_SL, HIGH);//turns off sleep mode and activates device

	digitalWrite(_xpin, HIGH);//turn on pull up resistor
	digitalWrite(_ypin, HIGH);//turn on pull up resistor
	digitalWrite(_zpin, HIGH);//turn on pull up resistor

	_calibrated = false;
}

//#####################################################

#endif // ACCELEROMETER_H
