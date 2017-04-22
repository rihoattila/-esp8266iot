/****************************************************************
 * ReadSHT25
 *  An example sketch that reads the sensor and prints the
 *  relative humidity to the PC's serial port
 *
 *  Tested with:
 *    - SHT25-Humidity sensor
 ***************************************************************/

#include <Wire.h>
#include <SHT25.h>


void setup()
{
  Wire.begin();
  Serial.begin(115200);
}

void loop()
{
  Serial.print("Humidity(%RH): ");
  Serial.print(SHT25.GetHumidity());
  Serial.print("     Temperature(C): ");
  Serial.println(SHT25.GetTemperature());
  Serial.print("     Dewpoint(C): ");
  Serial.println(SHT25.GetDewPoint());
  
  delay(1000);
}

