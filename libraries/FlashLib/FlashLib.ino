/*
  SST25VF016B Flash library.
 
  modified 20 Jan 2013
  by Luca Veronesi
 
  This code is in the public domain.
*/

#include <SPI.h> 

#include "SST25VF016B.h"
 
#define flash_CS  4  // Chip select on pin 4

// the setup routine runs once when you press reset:
void setup() 
{ 
  Serial.begin(57600);
  
  FLASHVF016.init(flash_CS);  // Chip select on pin 4
  Serial.println("Init flash"); 
  delay(50);
  
  /*
  FLASHVF016.writeStatusRegistry(0x00);
  Serial.println("Set status registry writeable"); 
  delay(50);
  */
  
  Serial.println("Read status registry"); 
  Serial.println(FLASHVF016.readStatusRegistry());
  delay(50);
  
  /*
  FLASHVF016.eraseChip();
  Serial.println("Erase Chip"); 
  delay(50);

  FLASHVF016.writeByte(0x30, 0x010203);
  Serial.println("Write some data"); 
  delay(5);
  FLASHVF016.writeByte(0x40, 0x010204);
  Serial.println("Write some data"); 
  delay(5);  
  */

  FLASHVF016.writeByte(0x40, 1020);
  Serial.println("Write some data"); 
  delay(5);  
  
  Serial.println("Read data"); 
  Serial.println(FLASHVF016.readByte(1020));
  delay(5);

  Serial.println("Read data"); 
  Serial.println(FLASHVF016.readByte(0x010204));
  delay(5);
  
  Serial.println("Read data multi"); 
  Serial.println(FLASHVF016.readByteMulti(0x010203, READ_BEGIN));
  Serial.println(FLASHVF016.readByteMulti(0x010203, READ_CONTINUE));  
  Serial.println(FLASHVF016.readByteMulti(0x010203, READ_CONTINUE));  
  Serial.println(FLASHVF016.readByteMulti(0x010203, READ_END));    
  delay(5);  
}

// the loop routine runs over and over again forever:
void loop() 
{
}


