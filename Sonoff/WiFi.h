#ifndef __WIFI_H
#define __WIFI_H

#include <ESP8266WiFi.h>

/* WiFi parameters */
#define WIFI_SSID "ssid";
#define WIFI_Password "password";
extern unsigned char wifiMac[6];

/* Host parameters */
#define WIFI_HOST_NAME_PREFIX "NodeMCU"
extern char *wifiHostname;

/* WiFi client */
extern WiFiClient wifiClient;

/* Common */
void wifiSetup();
void wifiLoop(unsigned long now);

#endif
