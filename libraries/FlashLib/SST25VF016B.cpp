/*
  SST25VF016B.cpp
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

#include "SST25VF016B.h"

SST25VF016B::SST25VF016B(void)
{
  
}

void SST25VF016B::init(byte slaveSelectPin)
{
  CE = slaveSelectPin;
  // start the SPI library:
  SPI.begin(CE);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(CE,CLOCK_DIVIDER);  
}
    
byte SST25VF016B::readByte(long address)
{
  byte read_byte = 0x00;
  byte address_b[4];
  address_b[0] = address;Serial.print("B0: ");Serial.println(address_b[0]);
  address_b[1] = address >> 8;Serial.print("B1: ");Serial.println(address_b[1]);
  address_b[2] = address >> 16;Serial.print("B2: ");Serial.println(address_b[2]);
  address_b[3] = address >> 24;Serial.print("B3: ");Serial.println(address_b[3]);
  SPI.transfer(CE, READ_FAST, SPI_CONTINUE);  // Read command
  SPI.transfer(CE, address_b[2], SPI_CONTINUE);  // Address MSB [A23-A16]
  SPI.transfer(CE, address_b[1], SPI_CONTINUE);  // Address MSB [A15-A8]
  SPI.transfer(CE, address_b[0], SPI_CONTINUE);  // Address MSB [A7-A0]  
  SPI.transfer(CE, 0x00, SPI_CONTINUE);  // Dummy byte
  read_byte =   SPI.transfer(CE, 0x00, SPI_LAST);  // Read byte formSPI bus
  return read_byte;
}

byte SST25VF016B::readByteMulti(long address, byte action)
{
  byte read_byte = 0x00;
  byte address_b[4];
  
  switch(action)
  {
    case READ_BEGIN:
      address_b[0] = address;
      address_b[1] = address >> 8;
      address_b[2] = address >> 16;
      address_b[3] = address >> 24;
      SPI.transfer(CE, READ_FAST, SPI_CONTINUE);  // Read command
      SPI.transfer(CE, address_b[2], SPI_CONTINUE);  // Address MSB [A23-A16]
      SPI.transfer(CE, address_b[1], SPI_CONTINUE);  // Address MSB [A15-A8]
      SPI.transfer(CE, address_b[0], SPI_CONTINUE);  // Address MSB [A7-A0]  
      SPI.transfer(CE, 0x00, SPI_CONTINUE);  // Dummy byte
      read_byte =   SPI.transfer(CE, 0x00, SPI_CONTINUE);  // Read byte formSPI bus
      break;
      
    case READ_CONTINUE:
      read_byte =   SPI.transfer(CE, 0x00, SPI_CONTINUE);  // Read byte formSPI bus
      break;

    case READ_END:
      read_byte =   SPI.transfer(CE, 0x00, SPI_LAST);  // Read byte formSPI bus
      break;
  }

  return read_byte;  
}

void SST25VF016B::writeByte(byte b, long address)
{
  byte address_b[4];
  address_b[0] = address;
  address_b[1] = address >> 8;
  address_b[2] = address >> 16;
  address_b[3] = address >> 24;
  write_enable();
  SPI.transfer(CE, BYTE_WRITE, SPI_CONTINUE);  // Read command
  SPI.transfer(CE, address_b[2], SPI_CONTINUE);  // Address MSB [A23-A16]
  SPI.transfer(CE, address_b[1], SPI_CONTINUE);  // Address MSB [A15-A8]
  SPI.transfer(CE, address_b[0], SPI_CONTINUE);  // Address MSB [A7-A0]  
  SPI.transfer(CE, b, SPI_LAST);  // Data byte
}


byte SST25VF016B::readStatusRegistry()
{
  byte statreg;
  SPI.transfer(CE, STATREG_READ, SPI_CONTINUE);
  statreg = SPI.transfer(CE, 0x00, SPI_LAST);  
  return statreg;
}

void SST25VF016B::writeStatusRegistry(byte registry)
{
  statreg_write_enable();
  SPI.transfer(CE, STATREG_WRITE, SPI_CONTINUE);
  SPI.transfer(CE, registry, SPI_LAST);
}

byte SST25VF016B::statreg_write_enable()
{
  SPI.transfer(CE, STATREG_WR_EN);
}    

    
void SST25VF016B::eraseChip(void)
{
  write_enable();
  SPI.transfer(CE, CHIP_ERASE);  // Chip-Erase
}

boolean SST25VF016B::isReady(void)
{
  byte statreg = readStatusRegistry();
  return bitRead(statreg,0);
}

void SST25VF016B::eraseSector(long sectorAddress, byte sectorSize)
{
  byte address_b[4];
  address_b[0] = sectorAddress;
  address_b[1] = sectorAddress >> 8;
  address_b[2] = sectorAddress >> 16;
  address_b[3] = sectorAddress >> 24;
  write_enable();
  switch(sectorSize)
  {
    case SECTOR_4KB:
      SPI.transfer(CE, SECTOR_ERASE_4K, SPI_CONTINUE);  // 4KB sector erase
      break;  

    case SECTOR_32KB:
      SPI.transfer(CE, SECTOR_ERASE_32K, SPI_CONTINUE);  // 32KB sector erase
      break;  

    case SECTOR_64KB:
      SPI.transfer(CE, SECTOR_ERASE_64K, SPI_CONTINUE);  // 64KB sector erase
      break;        
  } 
  SPI.transfer(CE, address_b[2], SPI_CONTINUE);  // Address MSB [A23-A16]
  SPI.transfer(CE, address_b[1], SPI_CONTINUE);  // Address MSB [A15-A8]
  SPI.transfer(CE, address_b[0], SPI_LAST);  // Address MSB [A7-A0]  
}


void SST25VF016B::readID(byte *id)
{
  SPI.transfer(CE, READ_ID, SPI_CONTINUE);  // JEDEC
  id[0] = SPI.transfer(CE, 0x00, SPI_CONTINUE);
  id[1] = SPI.transfer(CE, 0x00, SPI_CONTINUE);
  id[2] = SPI.transfer(CE, 0x00, SPI_LAST);
}

void SST25VF016B::write_enable()
{
  SPI.transfer(CE, WRITE_ENABLE);  // WREN
}

void SST25VF016B::write_disable()
{
  SPI.transfer(CE, WRITE_DISABLE);  // WRDI
}

SST25VF016B FLASHVF016;
