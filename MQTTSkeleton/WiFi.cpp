#include "WiFi.h"
#include "MQTT.h"
#include "IoT.h"
#include "string.h"

/* WiFi parameters */
unsigned char wifiMac[6];

/* Host parameters */
String wifiHostname = "";

/* WiFi client */
WiFiClient wifiClient;
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

/* Initialization handler */
void wifiSetup(const char *wifiSsid, const char *wifiPassword) {

  /* Initialize WiFi */
  WiFi.macAddress(wifiMac);
  WiFi.persistent(false);
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

  /* Initialize IoT */
  iotSetup(wifiMac);

  /* Set host name */
  char tmp[15];
  sprintf(tmp, WIFI_HOST_NAME_PREFIX, iotEuiShortString);
  wifiHostname = tmp;
  WiFi.hostname(wifiHostname);

  /*  event handlers*/
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
  {
    mqttWiFiConnected();
  });
  
  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
  {
    mqttWiFiDisconnected();
  });
  
  /* Done */
  WiFi.begin(wifiSsid, wifiPassword);
}

/* Main cycle handler */
void wifiLoop(unsigned long now) {
}

