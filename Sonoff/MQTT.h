#ifndef __MQTT_H
#define __MQTT_H

/* MQTT parameters */
//#define MQTT_SERVER "192.168.1.100"
#define MQTT_SERVER "iot.smarthome.com"
#define MQTT_PORT 1883

/* State machine */
#define MQTT_STATE_WAITING_FOR_IP_ADDRESS 0
#define MQTT_STATE_WAITING_FOR_MQTT_CONNECTED 1
#define MQTT_STATE_WAITING_FOR_MQTT_ACTIONS 2
extern unsigned char mqttState;

/* Services */
void mqttWiFiConnected();
void mqttWiFiDisconnected();
void mqttDebug(char *debugTopic, char *debugText);

/* Common */
void mqttSetup();
void mqttLoop(unsigned long now);

#endif

