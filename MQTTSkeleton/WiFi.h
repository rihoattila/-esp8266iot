#ifndef __WIFI_H
#define __WIFI_H

#include <ESP8266WiFi.h>

/* WiFi parameters */
extern unsigned char wifiMac[6];

/* Host parameters */
#define WIFI_HOST_NAME_PREFIX "IoT-%s"
extern String wifiHostname;

/* WiFi client */
extern WiFiClient wifiClient;

/* Common */
void wifiSetup(const char *wifiSsid, const char *wifiPassword);
void wifiLoop(unsigned long now);

#endif
