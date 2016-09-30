/*

Arduino Library for Texas Instruments HDC1080 Digital Humidity and Temperature Sensor
Written by AA for ClosedCube
---

The MIT License (MIT)

Copyright (c) 2016 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <Wire.h>

#include "TMP112.h"


TMP112::TMP112()
{
}

void TMP112::begin(uint8_t address) {
	_address = address;
	Wire.begin();

	Wire.beginTransmission(_address);
	Wire.write(0x00);
	Wire.endTransmission();

}

float TMP112::readT() {
	return readTemperature();
}

float TMP112::readTemperature() {
	uint16_t rawT = readData(0);
	return ((rawT >> 4) & 0x0FFF) * 0.0625;
}

uint16_t TMP112::readData(uint8_t pointer) {
	Wire.beginTransmission(_address);
	Wire.write(pointer);
	Wire.endTransmission();
	
	delay(10);
	Wire.requestFrom(_address, 2);

	byte msb = Wire.read();
	byte lsb = Wire.read();

	return msb << 8 | lsb;
}



