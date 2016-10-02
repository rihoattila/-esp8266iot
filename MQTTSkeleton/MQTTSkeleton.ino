#include "FOTA.h"
#include "WebConfig/WebConfig.h"
#include "MQTT.h"
#include "IOT.h"

void setup() {
    // setup IOT
    createEUID();

    // setup web config
    setupWebConfig(EUID);

    // setup FOTA
    setupFOTA();

    // setup MQTT
    setupMQTT(EUID, config.ssid.c_str(), config.password.c_str());

    //**** Normal Sketch code here...
      
   
}

// the loop function runs over and over again forever
void loop() {
  // OTA request handling
  handleFOTA();
  
  //  WebServer requests handling
  handleWebConfig();
  
  // handle MQTT thing
  handleMQTT();

}

