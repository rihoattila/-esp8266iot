/**
 @file FaBoTemperature_ADT7410.cpp
 @brief This is a library for the FaBo Temperature I2C Brick.

   http://fabo.io/207.html

   Released under APACHE LICENSE, VERSION 2.0

   http://www.apache.org/licenses/

 @author FaBo<info@fabo.io>
*/

#include "FaBoTemperature_ADT7410.h"

/**
 @brief Constructor
*/
FaBoTemperature::FaBoTemperature(uint8_t addr) {
  _i2caddr = addr;
  Wire.begin();
}

/**
 @brief Begin Device
*/
void FaBoTemperature::begin() {
  uint8_t config;
//   config  = ADT7410_13BIT_RESOLUTION;
  config  = ADT7410_16BIT_RESOLUTION;
  config |= ADT7410_16BIT_OP_MODE_1FAULT;
  config |= ADT7410_CT_LOW;
  config |= ADT7410_INT_LOW;
  config |= ADT7410_INTERRUPT_MODE;
  config |= ADT7410_OP_MODE_CONTINUOUS;
  configure(config);
}

/**
 @brief Configure Device
 @param [in] config Configure Parameter
*/
void FaBoTemperature::configure(uint8_t config) {
  writeI2c(ADT7410_CONFIGURATION_REG, config);
}

/**
 @brief Data Ready Check
 @retval true Data ready
 @retval false Data Not ready
*/
bool FaBoTemperature::checkDataReady() {
  uint8_t status;
  readI2c(ADT7410_STATUS_REG, 1, &status);
  if ( (status & 0x80) ) {
    return false;
  } else {
    return true;
  }
}

/**
 @brief Read Temperature Data
 @param [out] value Temperature Data
*/
float FaBoTemperature::readTemperature() {
  uint8_t data;
  uint8_t buffer[2];
  uint16_t adc;
  float temp;
  int16_t val;
  uint8_t config;

  if ( checkDataReady() ) {

    readI2c(ADT7410_CONFIGURATION_REG, 1, &config);
    readI2c(ADT7410_TEMP_MSB_REG, 2, buffer);

    adc = (uint16_t)buffer[0] << 8;
    adc |= buffer[1];
    val = (long)adc;

    if ( (config & 0x80) ) {
      // 13bit resolution
      adc >>= 3;
      if(adc & 0x1000) {   // 符号の判定
        val = val - 8192;  // マイナスの場合
      }
      temp = (float)val / 16.0;
    } else {
      // 16bit resolution
      if(adc & 0x8000) {    // 符号の判定
        val = val - 65536;  // マイナスの場合
      }
      temp = (float)val / 128.0;
    }
    return temp;
  } else {
    return 0.0;
  }
}



/**
 @brief Write I2C
 @param [in] address register address
 @param [in] data write data
*/
void FaBoTemperature::writeI2c(uint8_t address, uint8_t data) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

/**
 @brief Read I2C
 @param [in] address register address
 @param [in] num read length
 @param [out] data read data
*/
void FaBoTemperature::readI2c(uint8_t address, uint8_t num, uint8_t * data) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(address);
  Wire.endTransmission();
  uint8_t i = 0;
  Wire.requestFrom(_i2caddr, num);
  while( Wire.available() ) {
    data[i++] = Wire.read();
  }
}
