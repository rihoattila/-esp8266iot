#include <PubSubClient.h>

#include "WiFi.h"
#include "IoT.h"
#include "MQTT.h"

/* MQTT parameters */
char* mqttServer = (char *)"0.0.0.0";
unsigned int mqttPort = 1883;
std::function<void(char*, char*, unsigned int)> _mqttcallback;
std::function<void()> _mqttreconnectcallback;

/* State machine */
unsigned char mqttState = MQTT_STATE_WAITING_FOR_IP_ADDRESS;

/* Topic paths */
char mqttTopicDebug[23] = "0000000000000000/debug";

/* MQTT connection */
PubSubClient mqttClient(wifiClient);
unsigned long mqttReconnectLast = 0;
unsigned long mqttReconnectPeriod = 3000;

/* Reconnection */
void mqttReconnect() {
  if (!mqttClient.connected()) {
    if (mqttClient.connect(iotEuiString)) {
      mqttClient.publish(mqttTopicDebug, "MQTT connected");
      IPAddress ipAddress = WiFi.localIP();
      char b[30];
      sprintf(b, "%d:%d:%d:%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
      mqttClient.publish(mqttTopicDebug, b);

      if(_mqttreconnectcallback)
      {
        _mqttreconnectcallback();
      }
    }
  }
}

void mqttSubscribe(char* topic)
{
  mqttClient.subscribe(topic);
}

/* Publish-subscribe client callback */
void mqttPubSubCallback(char* topic, byte* payload, unsigned int length)
{
  char* localPayload = new char[length + 1];
  int i;
  for(i = 0; i < length; i++)
  {
    localPayload[i] = (char)payload[i];
  }
  localPayload[i] = 0;
  
  mqttClient.publish(mqttTopicDebug, "MQTT received");
  
  if(_mqttcallback)
  {
    _mqttcallback(topic,localPayload,length);
  }
}

/* Wi-Fi connected handler */
void mqttWiFiConnected() {
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttPubSubCallback);
  mqttReconnectLast = 0;
  mqttState = MQTT_STATE_WAITING_FOR_MQTT_CONNECTED;
}

/* Wi-Fi disconnected handler */
void mqttWiFiDisconnected() {
  mqttState = MQTT_STATE_WAITING_FOR_IP_ADDRESS;
}

/* Debugging */
void mqttDebug(const char *debugText) {
  mqttClient.publish(mqttTopicDebug, debugText);
}

/* Initialization handler */
void mqttSetup(char * MQTT_SERVER, const unsigned int MQTT_PORT) {

  mqttServer = MQTT_SERVER;
  mqttPort = MQTT_PORT;

  /* Generate topic paths */
  for (unsigned char i = 0; i < 16; i++) {
    char b = iotEuiString[i];
    mqttTopicDebug[i] = b;
  }
}

void mqttSetupTopic(char* topic) {

  /* Generate topic paths */
  for (unsigned char i = 0; i < 16; i++) {
    char b = iotEuiString[i];
    topic[i] = b;
  }
}

/* Main cycle handler */
void mqttLoop(unsigned long now) {

  /* Handle state machine */
  if (mqttState == MQTT_STATE_WAITING_FOR_MQTT_CONNECTED) {
    if (!mqttClient.connected()) {
      if (mqttReconnectPeriod < (now - mqttReconnectLast)) {
        mqttReconnectLast = now;
        mqttReconnect();
      }
    }
    else {
      mqttState = MQTT_STATE_WAITING_FOR_MQTT_ACTIONS;
    }
  }
  else {
    if (mqttState == MQTT_STATE_WAITING_FOR_MQTT_ACTIONS) {
      if (mqttClient.connected()) {
        mqttClient.loop();
      }
      else {
        mqttState = MQTT_STATE_WAITING_FOR_MQTT_CONNECTED;
      }
    }
  }
}

/* Sending State */
void mqttPublish(char *topic, char *payload)
{
  mqttClient.publish(topic, payload);
}


void setPublishCallback(std::function<void(char*, char*, unsigned int)> callback)
{
  _mqttcallback = callback;
}

void setReconnectCallback(std::function<void()> callback)
{
  _mqttreconnectcallback = callback;
}
