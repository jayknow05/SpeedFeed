/*
  SST25VF016B.h
  FLASHVF016 Library for Arduino 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  US

  This library is based on several projects:
 
  The Arduino EEPROM library found here:  
  http://arduino.cc/en/Reference/EEPROM

 
*/
#ifndef SST25VF016B_h
#define SST25VF016B_h

#include <Arduino.h>
#include <SPI.h>

// Flash module commands:
const byte BYTE_WRITE =       0x02;  // Command to program a byte
const byte READ_SLOW =        0x03;  // Read command
const byte READ_FAST =        0x0B;  // Read command  
const byte WRITE_ENABLE =     0x06;  // Write enable command
const byte WRITE_DISABLE =    0x04;  // Write disable command
const byte CHIP_ERASE =       0xC7;  // Command to erase entire chip
const byte SECTOR_ERASE_4K =  0x20;  // Command to erase 4 KByte of memory array
const byte SECTOR_ERASE_32K = 0x52;  // Command to erase 4 KByte of memory array
const byte SECTOR_ERASE_64K = 0xD8;  // Command to erase 4 KByte of memory array
const byte STATREG_READ =     0x05;  // Command to read status registry
const byte READ_ID =          0x9F;  // Command to read ID (JEDEC)
const byte STATREG_WR_EN =    0x50;  // Command to set write enable on status registry
const byte STATREG_WRITE =    0x01;  // Command to write status registry

// Clock divider
const byte CLOCK_DIVIDER =  2;     // Clock divider (84MHz/CLOCK_DIVIDER=21Mhz operations clock)

#define  SECTOR_4KB    0
#define  SECTOR_32KB   1
#define  SECTOR_64KB   2
#define  READ_BEGIN    3
#define  READ_CONTINUE 4
#define  READ_END      5



class SST25VF016B
{
  public:
    SST25VF016B();
    void init(byte slaveSelectPin);
    void eraseChip();
    void eraseSector(long sectorAddress, byte sectorSize = SECTOR_4KB);
    boolean isReady();
    byte readByte(long address);
    byte readByteMulti(long address, byte action = READ_BEGIN);
    void writeByte(byte b, long address);
    void readID(byte *id);        
    byte readStatusRegistry();
    void writeStatusRegistry(byte registry);    
    
  private:    
    byte statreg_write_enable();    
    void write_enable();
    void write_disable();
  private:
    byte CE;  // Chip enable (SPI Chip Select) pin 
};

extern SST25VF016B FLASHVF016;

#endif
