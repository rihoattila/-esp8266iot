#ifndef __MQTT_H
#define __MQTT_H
#include <functional>

/* State machine */
#define MQTT_STATE_WAITING_FOR_IP_ADDRESS 0
#define MQTT_STATE_WAITING_FOR_MQTT_CONNECTED 1
#define MQTT_STATE_WAITING_FOR_MQTT_ACTIONS 2
extern unsigned char mqttState;

/* Services */
void mqttWiFiConnected();
void mqttWiFiDisconnected();
void mqttSubscribe(char* topic);
void mqttPublish(char *topic, char *payload);
void mqttDebug(const char *debugText);
void setPublishCallback(std::function<void(char*, char*, unsigned int)> callback);
void setReconnectCallback(std::function<void()> callback);

/* Common */
void mqttSetup(char * MQTT_SERVER, const unsigned int MQTT_PORT);
void mqttSetupTopic(char* topic);
void mqttLoop(unsigned long now);

#endif

