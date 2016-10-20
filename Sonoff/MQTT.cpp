#include <PubSubClient.h>

#include "WiFi.h"
#include "IoT.h"
#include "MQTT.h"

/* MQTT parameters */
const char* mqttServer = MQTT_SERVER;
const unsigned int mqttPort = MQTT_PORT;

/* State machine */
unsigned char mqttState = MQTT_STATE_WAITING_FOR_IP_ADDRESS;

/* Topic paths */
char *mqttTopicDebug = "0000000000000000/debug";
char *mqttTopicState = "0000000000000000/state";
char *mqttTopicSwitch = "0000000000000000/switch";

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
      mqttClient.subscribe(mqttTopicSwitch);
    }
  }
}

/* Publish-subscribe client callback */
void mqttPubSubCallback(char* topic, byte* payload, unsigned int length) {
  mqttClient.publish(mqttTopicDebug, "MQTT received");
  if (2 <= length) {
    char b = payload[1];
    if (length == 2) {
      //@@      pinRelayState = 1;
      mqttClient.publish(mqttTopicDebug, "switch ON");
    }
    else {
      //@@      pinRelayState = 0;
      mqttClient.publish(mqttTopicDebug, "switch OFF");
    }
    //@@    digitalWrite(pinRelay, (pinRelayState ? HIGH : LOW));
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
void mqttDebug(char *debugTopic, char *debugText) {
  mqttClient.publish(debugTopic, debugText);
}

/* Initialization handler */
void mqttSetup() {

  /* Generate topic paths */
  for (unsigned char i = 0; i < 16; i++) {
    char b = iotEuiString[i];
    mqttTopicDebug[i] = b;
    mqttTopicState[i] = b;
    mqttTopicSwitch[i] = b;
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

