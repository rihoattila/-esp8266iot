/**
 @file FaBoTemperature_ADT7410.h
 @brief This is a library for the FaBo Temperature I2C Brick.

   http://fabo.io/207.html

   Released under APACHE LICENSE, VERSION 2.0

   http://www.apache.org/licenses/

 @author FaBo<info@fabo.io>
*/

#ifndef FABOTEMPERATURE_ADT7410_H
#define FABOTEMPERATURE_ADT7410_H

#include <Arduino.h>
#include <Wire.h>

#define ADT7410_SLAVE_ADDRESS 0x48 ///< Default I2C Slave Address

/// @name Register Addresses
/// @{
#define ADT7410_TEMP_MSB_REG 0x00
#define ADT7410_TEMP_LSB_REG 0x01
#define ADT7410_STATUS_REG 0x02
#define ADT7410_CONFIGURATION_REG 0x03
#define ADT7410_WHO_AM_I_REG 0x0B
/// @}

/// @name Config Parameter
/// @{
#define ADT7410_13BIT_RESOLUTION 0b00000000
#define ADT7410_16BIT_RESOLUTION 0b10000000
#define ADT7410_OP_MODE_CONTINUOUS 0b00000000
#define ADT7410_OP_MODE_ONESHOT 0b00100000
#define ADT7410_OP_MODE_SPS 0b01000000
#define ADT7410_OP_MODE_SHUTDOWN 0b01100000
#define ADT7410_INTERRUPT_MODE  0b00000000
#define ADT7410_COMPARATOR_MODE 0b00010000
#define ADT7410_INT_LOW  0b00000000
#define ADT7410_INT_HIGH 0b00001000
#define ADT7410_CT_LOW  0b00000000
#define ADT7410_CT_HIGH 0b00000100
#define ADT7410_16BIT_OP_MODE_1FAULT 0b00
#define ADT7410_16BIT_OP_MODE_2FAULT 0b01
#define ADT7410_16BIT_OP_MODE_3FAULT 0b10
#define ADT7410_16BIT_OP_MODE_4FAULT 0b11
/// @}


/**
 @class FaBoTemperature
 @brief FaBo Temperature I2C Controll class
*/
class FaBoTemperature {
  public:
    FaBoTemperature(uint8_t addr = ADT7410_SLAVE_ADDRESS);
    void begin(void);
    void configure(uint8_t config);
    bool checkDataReady(void);
    float readTemperature(void);
  private:
    uint8_t _i2caddr;
    void writeI2c(uint8_t address, uint8_t data);
    void readI2c(uint8_t address, uint8_t num, uint8_t * data);
};

#endif // FABOTEMPERATURE_ADT7410_H
