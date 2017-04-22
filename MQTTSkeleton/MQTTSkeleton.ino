#include <ArduinoJson.h>

#include "WiFi.h"
#include "FOTA.h"
#include "IoT.h"
#include "MQTT.h"

/* WIFI parameters */
#define WIFI_SSID "WiFiRA"
#define WIFI_Password "Blahhala"

/* MQTT parameters */
#define MQTT_SERVER (char *)"192.168.1.252"
#define MQTT_PORT 1883

char mqttTopicGetState[26] = "0000000000000000/getstate";

char msg[100];

void setup()
{
  
  /* Initialize WiFi */
  wifiSetup(WIFI_SSID, WIFI_Password);

  /* Initialize FOTA */
  fotaSetup(wifiHostname);

  /* Initialize MQTT */
  mqttSetup(MQTT_SERVER, MQTT_PORT);
  mqttSetupTopic(mqttTopicGetState);
  
  mqttSubscribe(mqttTopicGetState);

  setPublishCallback(mqttPublishReceived);
  setReconnectCallback(mqttReconnected);
      
}

int compare(char* a, char* b)
{
  int ret = 0;
  if(strlen(a) == strlen(b))
  {
    for(int i = 0; i < strlen(a); i++)
    {
      if(a[i] != b[i])
      {
        ret = i;
        break;
      }
    }
  }
  else
  {
    ret = -1;
  }
  return ret;
}

void mqttReconnected()
{
  mqttSubscribe(mqttTopicGetState);
}

void mqttPublishReceived(char* topic, char* payload, unsigned int length)
{
}

// the loop function runs over and over again forever
void loop() {

  unsigned long now = millis();

  /* Handle WiFi */
  wifiLoop(now);

  /* Handle FOTA */
  fotaLoop(now);

  /* Handle IoT */
  iotLoop(now);

  /* Handle MQTT */
  mqttLoop(now);

}

