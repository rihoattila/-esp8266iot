#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* State machine */
#define STATE_WAITING_FOR_IP_ADDRESS 0
#define STATE_WAITING_FOR_MQTT_CONNECTED 1
#define STATE_WAITING_FOR_MQTT_ACTIONS 3
unsigned char state = STATE_WAITING_FOR_IP_ADDRESS;

/* WiFi parameters */
WiFiClient wifiClient;
byte wifiMac[6];
const char* hostName = "iotGL3";

/* Pins */
const int pinLed = 13;
const int pinRelay = 12;
unsigned char pinRelayState = 0;

/* MQTT parameters */
#define MQTTPUBLISHINTERVAL 6000
long lastMQTTPublish = -1 * MQTTPUBLISHINTERVAL;
byte mqttClientIdRaw[8];
const char* mqttClientId = "0000000000000000";
const char* mqttServer = "192.168.1.252";
const unsigned int mqttPort = 1883;
const byte mqttprefixlength = 6;
char *mqttTopicDebug = "ha/v1/0000000000000000/debug";
char *mqttTopicState = "ha/v1/0000000000000000/state";
char *mqttTopicSwitch = "ha/v1/0000000000000000/switch";

/* MQTT connection */
PubSubClient  mqttClient(wifiClient);
unsigned long mqttReconnectLast = 0;
unsigned long mqttReconnectPeriod = 2000;


void mqttCallback(char* topic, byte* payload, unsigned int length){
  mqttClient.publish(mqttTopicDebug, "MQTT received");
  if(2 <= length){
    char b = payload[1];
    if(length == 2){
      pinRelayState = 1;
      mqttClient.publish(mqttTopicDebug, "switch ON");
    }
    else{
      pinRelayState = 0;
      mqttClient.publish(mqttTopicDebug, "switch OFF");
    }
    digitalWrite(pinRelay, (pinRelayState ? HIGH : LOW));
  }
}

/* WiFi events */
void WiFiEvent(WiFiEvent_t event){
  switch(event){
    case WIFI_EVENT_STAMODE_GOT_IP:
      mqttClient.setServer(mqttServer, mqttPort);
      mqttClient.setCallback(mqttCallback);
      mqttReconnectLast = millis();
      state = STATE_WAITING_FOR_MQTT_CONNECTED;
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      state = STATE_WAITING_FOR_IP_ADDRESS;
      break;
  }
}

void setupMQTT(String EUID,const char* wifiSsid, const char* wifiPassword){
  //WiFi.macAddress(wifiMac);
  //WiFi.persistent(false);
  //WiFi.setAutoConnect(true);
  //WiFi.mode(WIFI_STA);
  //WiFi.hostname(hostName);
  //WiFi.onEvent(WiFiEvent);
  //WiFi.begin(wifiSsid, wifiPassword);

  mqttClientId = EUID.c_str();
  for(unsigned char i = 0; i < 16; i++){
    byte b = mqttClientId[i];
	  mqttTopicDebug[mqttprefixlength + i] = b;
	  mqttTopicState[mqttprefixlength + i] = b;
	  mqttTopicSwitch[mqttprefixlength + i] = b;
  }
}

void mqttReconnect(){
  if(!mqttClient.connected()){
    if(mqttClient.connect(mqttClientId)){
      mqttClient.publish(mqttTopicDebug, "MQTT connected");
      mqttClient.subscribe(mqttTopicSwitch);
    }
  }
}


void mqttSendState()
{
  char msg[100];

  StaticJsonBuffer<200> jsonBuffer1;
  JsonObject& root = jsonBuffer1.createObject();

  root["temp"] = "0";

  root.printTo(msg, sizeof(msg));
  mqttClient.publish(mqttTopicState, msg);

}

void handleMQTT(){
  unsigned long now = millis();
  
  if(state == STATE_WAITING_FOR_IP_ADDRESS)
  {
    switch(WiFi.status())
    {
      case WL_CONNECTED:
        mqttClient.setServer(mqttServer, mqttPort);
        mqttClient.setCallback(mqttCallback);
        mqttReconnectLast = millis();
        state = STATE_WAITING_FOR_MQTT_CONNECTED;
        break;
    }
  }
  /* Handle MQTT */
  else if(state == STATE_WAITING_FOR_MQTT_CONNECTED){
    if(!mqttClient.connected()){
      if(mqttReconnectPeriod < (now - mqttReconnectLast)){
        mqttReconnectLast = now;
        mqttReconnect();
      }
    }
    else{
      state = STATE_WAITING_FOR_MQTT_ACTIONS;
    }
  }
  else{
    if(state == STATE_WAITING_FOR_MQTT_ACTIONS){
      if(mqttClient.connected()){
        mqttClient.loop();
        
        if (MQTTPUBLISHINTERVAL < (now - lastMQTTPublish)){
            lastMQTTPublish = now;
            
            mqttSendState();
  
        }
      }
      else{
        state = STATE_WAITING_FOR_MQTT_CONNECTED;
      }
    }
  }
}

