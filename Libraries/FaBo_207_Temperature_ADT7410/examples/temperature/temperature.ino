/**
 @file temperature.ino
 @brief This is an Example for the FaBo Temperature I2C Brick.

   http://fabo.io/207.html

   Released under APACHE LICENSE, VERSION 2.0

   http://www.apache.org/licenses/

 @author FaBo<info@fabo.io>
*/

#include <Wire.h>
#include <FaBoTemperature_ADT7410.h>

FaBoTemperature faboTemperature;

void setup() {
  Serial.begin(9600);
  Serial.println("RESET");
  Serial.println();

  faboTemperature.begin();
}

void loop() {
  Serial.print("Temperarue: ");
  Serial.println(faboTemperature.readTemperature(),8);
  delay(1000);
}
