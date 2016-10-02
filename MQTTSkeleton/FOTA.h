#include <ArduinoOTA.h>

#define LED_esp 2

void handleFOTA(){
  ArduinoOTA.handle();
}

void setupFOTA(){
	// ArduinoOTA.setHostname(host);
    // ArduinoOTA.onStart([]() { // what to do before OTA download insert code here
        // Serial.println("Start");
      // });
    // ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    // });
    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        for (int i=0;i<30;i++){
          analogWrite(LED_esp,(i*100) % 1001);
          delay(50);
        }
        digitalWrite(LED_esp,HIGH); // Switch OFF ESP LED to save energy
        ESP.restart();
      });
  
    ArduinoOTA.onError([](ota_error_t error) { 
//        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        ESP.restart(); 
      });
  
     /* setup the OTA server */
    ArduinoOTA.begin();
}