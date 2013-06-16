/*####################################################################
 FILE: Accelerometer.h
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
#define ACCELEROMETER_H

#include <stddef.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif

/*
  This will pull the topmost Versalino library for the
  BUS layouts, VersalinoPINS and VersalinoBUS structures
  that you will be using in this library.

  A version of the Versalino.h should always be included
  in  your library folder just in case the user has not
  installed a Versalino library. (This will prevent compile
  errors, and/or lower the complexity of the code you need
  to write for a cross compatible Library)

  using <Versalino.h> as apposed to "Versalino.h" is what
  makes this code automatically pull from a higher level if
  the library is already present in the users libraries folder.
*/
#include <Versalino.h>
#include <inttypes.h>
//any other needed includes should go here.


/*
  This is where your class structure is established
  I have included a standard structure that establishes
  methods to ensure your code can take advantage of the
  special features of the Versalino, while still being
  flexible enough to take pin level role assignments.

  That is to say you should generally provide two or more
  initialization methods: One which takes the versalinoBUS
  that the user will be operating on as part of its input,
  and a second that takes explicit pin assignments.

  Obviously for some more complex devices or proprietary
  library uses it may be okay to simply use the versalinoBUS
  method, it would simply mean that the pins used would not
  be interchangeable.

  The choice of implementation is up to you, but I will do
  my best to provide you with all the tools you will need.
*/



class Accelerometer
{
	/*
	  The begin overloads you choose to use will be the primary
	  compatability layer for users that may use this library
	  interchangably between device and loadboard types.

	  This may effect your decision on what implementation methods
	  to support, but for ease of use, and BUS to BUS
	  interchangability the first -> "begin(VersalinoBUS myBUS);"
	  method must be included at a minimum.
	*/

	public:

	//void begin(uint8_t myPINS[9]);
	Accelerometer();
	Accelerometer(uint8_t mySL, uint8_t myGS, uint8_t my0G, uint8_t myXPIN, uint8_t myYPIN, uint8_t myZPIN);
	Accelerometer(VersalinoBUS myBus);

	void begin(VersalinoBUS myBUS);
	void begin(uint8_t mySL, uint8_t myGS, uint8_t my0G, uint8_t myXPIN, uint8_t myYPIN, uint8_t myZPIN);

	//-------functions
	//VersalinoBUS getBUS(); //This will return the VersalinoBUS currently assigned to the instance of this library
	void setBUS(VersalinoBUS myBus);


	//-------DC Motor Functions
	void calibrate();
	void read();
	void read(uint8_t numberOfReads);
	int readAverage(int pin, int samples);

	float convert(float myReading, float my0G);
	float convert(float myReading, float my0G, bool myUnits);

	//----------end constants
	/*
	This is where you would write your code to actually perform the task that you intended to with your library.
	*/

	unsigned int _Xraw;
	unsigned int _Yraw;
	unsigned int _Zraw;

	float _Xgs;
	float _Ygs;
	float _Zgs;

	int _0z;
	int _0y;
	int _0x;

	private:
	//VersalinoBUS _myBUS;
	//bool _busEnabled;

	uint8_t _zpin;
	uint8_t _ypin;
	uint8_t _xpin;
	uint8_t _0G;
	uint8_t _SL;
	uint8_t _GS;



	bool _calibrated;

	const static int _gc = 92;

};

	const static byte NOPIN = 255;
	const static boolean GFORCE = true;
	const static boolean METERS = false;
#endif // ACCELEROMETER_H
