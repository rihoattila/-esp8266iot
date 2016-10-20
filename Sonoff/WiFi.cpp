#include "WiFi.h"
#include "MQTT.h"

/* WiFi parameters */
const char *wifiSsid = WIFI_SSID;
const char *wifiPassword = WIFI_Password;
unsigned char wifiMac[6];

/* Host parameters */
char *wifiHostnamePrefix = WIFI_HOST_NAME_PREFIX;
char *wifiHostname;

/* WiFi client */
WiFiClient wifiClient;

/* WiFi event */
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
      mqttWiFiConnected();
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      mqttWiFiDisconnected();
      break;
  }
}

/* Initialization handler */
void wifiSetup() {

  /* Initialize WiFi */
  WiFi.macAddress(wifiMac);
  WiFi.persistent(false);
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiEvent);

  /* Set host name */
  WiFi.hostname(wifiHostnamePrefix);

  /* Done */
  WiFi.begin(wifiSsid, wifiPassword);
}

/* Main cycle handler */
void wifiLoop(unsigned long now) {
}

