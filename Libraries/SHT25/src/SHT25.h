/*
  SHT25 - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT25
 */


#ifndef SHT25_H
#define SHT25_H

#include <stdint.h>

class SHT25Class
{
  private:
    uint16_t readSensor(uint8_t command);

  public:
    float GetHumidity(void);
    float GetTemperature(void);
    float GetDewPoint(void);
};

extern SHT25Class SHT25;

#endif
