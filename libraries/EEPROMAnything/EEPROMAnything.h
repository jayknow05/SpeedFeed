#include <Arduino.h>  // for type definitions

#ifndef _EEPROM_ANYTHING_H
#define _EEPROM_ANYTHING_H

void writeEEPROM(int deviceaddress, int eeaddress, byte data );
byte readEEPROM(int deviceaddress, int eeaddress );

template <class T> int EEPROM_writeAnything(int deviceaddress, int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++) {
          writeEEPROM(deviceaddress, ee++, *p++);
          //Serial.print("WRITE");Serial.print(" ");Serial.print(deviceaddress); Serial.print(" "); Serial.print(ee); Serial.print(" "); Serial.println(*p);
      }
    return i;
}

template <class T> int EEPROM_readAnything(int deviceaddress, int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++) {
          *p++ = readEEPROM(deviceaddress, ee++);
          //Serial.print("READ");Serial.print(" ");Serial.print(deviceaddress); Serial.print(" "); Serial.print(ee); Serial.print(" "); Serial.println(*p);
      }
    return i;
}

#endif