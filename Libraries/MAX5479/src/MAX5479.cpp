#include <Wire.h>

#include "MAX5479.h"


MAX5479::MAX5479()
{
}

void MAX5479::begin(uint8_t address)
{
	_address = address;
	Wire.begin();
  Wire.setClock(400000L);
/* 	Wire.beginTransmission(_address);
	Wire.write(CONFIGURATION);
	Wire.write(0x0);
	Wire.write(0x0);
	Wire.endTransmission(); */

}

void MAX5479::writeR(uint8_t *data, uint8_t length)
{
	
  Wire.beginTransmission(_address);
  Wire.write(data, length);
//  Serial.println(_address);
//  
//  for ( uint8_t i = 0; i < length; i++ ) {
//    Wire.write( data[i] );
//    Serial.println(data[i]);
//  }
//  
  Wire.endTransmission();
}

