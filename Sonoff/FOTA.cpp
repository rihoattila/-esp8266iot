#include <ArduinoOTA.h>

#include "FOTA.h"
#include "MQTT.h"

/* Initialization handler */
void fotaSetup(char *hostName) {

  /* Enable OTA */
  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.onStart([]() {
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  });
  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    ESP.restart();
  });
  ArduinoOTA.begin();
}

/* Main cycle handler */
void fotaLoop(unsigned long now) {

  /* Handle OTA */
  ArduinoOTA.handle();
}

