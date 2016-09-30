#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <PubSubClient.h>

/* Common */
const char * hex = "0123456789ABCDEF";

/* State machine */
#define STATE_WAITING_FOR_IP_ADDRESS 0
#define STATE_WAITING_FOR_MQTT_CONNECTED 1
#define STATE_WAITING_FOR_MQTT_ACTIONS 3
unsigned char state = STATE_WAITING_FOR_IP_ADDRESS;

/* Pins */
const int pinLed = 13;
const int pinRelay = 12;
unsigned char pinRelayState = 0;

/* Host parameters */
const char* hostName = "";

/* WiFi parameters */
WiFiClient wifiClient;
const char* wifiSsid = "";
const char* wifiPassword = "";
byte wifiMac[6];

/* MQTT parameters */
byte mqttClientIdRaw[8];
char* mqttClientId = "0000000000000000";
const char* mqttServer = "192.168.1.100";
const unsigned int mqttPort = 1883;
char *mqttTopicDebug = "0000000000000000/debug";
char *mqttTopicState = "0000000000000000/state";
char *mqttTopicSwitch = "0000000000000000/switch";

/* Tickers */
Ticker tickerMain;

/* LED handler */
#define LED_PERIOD 200
unsigned char ledCounter = 0;

void LedTicker(){
  unsigned char ledActive = LED_PERIOD;
  switch(state){
    case STATE_WAITING_FOR_IP_ADDRESS:
      ledActive = LED_PERIOD - 10;
      break;
    case STATE_WAITING_FOR_MQTT_CONNECTED:
      ledActive = LED_PERIOD >> 1;
      break;
    case STATE_WAITING_FOR_MQTT_ACTIONS:
      ledActive = 3;
      break;
  }
  if(ledCounter < ledActive){
    digitalWrite(pinLed, LOW);
  }
  else{
    digitalWrite(pinLed, HIGH);
  }
  if(ledCounter < (LED_PERIOD - 1)){
    ledCounter++;
  }
  else{
    ledCounter = 0;
  }
}

/* MQTT connection */
PubSubClient mqttClient(wifiClient);
unsigned long mqttReconnectLast = 0;
unsigned long mqttReconnectPeriod = 2000;

void mqttReconnect(){
  if(!mqttClient.connected()){
    if(mqttClient.connect(mqttClientId)){
      mqttClient.publish(mqttTopicDebug, "MQTT connected");
      mqttClient.subscribe(mqttTopicSwitch);
    }
  }
}

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
      {
        mqttClientIdRaw[0] = wifiMac[0] ^ 0x02;
        mqttClientIdRaw[1] = wifiMac[1];
        mqttClientIdRaw[2] = wifiMac[2];
        mqttClientIdRaw[3] = 0xff;
        mqttClientIdRaw[4] = 0xfe;
        mqttClientIdRaw[5] = wifiMac[3];
        mqttClientIdRaw[6] = wifiMac[4];
        mqttClientIdRaw[7] = wifiMac[5];
        for(unsigned char i = 0; i < 8; i++){
          unsigned char j = i << 1;
          mqttClientId[j] = hex[mqttClientIdRaw[i] >> 4];
          mqttClientId[j + 1] = hex[mqttClientIdRaw[i] & 0x0f];
        }
        for(unsigned char i = 0; i < 16; i++){
          byte b = mqttClientId[i];
          mqttTopicDebug[i] = b;
          mqttTopicState[i] = b;
          mqttTopicSwitch[i] = b;
        }
      }
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

/* Initialization */
void setup(){

  /* Initialize pins */
  pinMode(pinLed, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinLed, LOW);
  digitalWrite(pinRelay, LOW);

  /* Initialize WiFi */
  WiFi.macAddress(wifiMac);
  WiFi.persistent(false);
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostName);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(wifiSsid, wifiPassword);

  /* Initialize tickers */
  tickerMain.attach_ms(10, LedTicker);
}

/* Main cycle */
void loop(){
  unsigned long now = millis();

  /* Handle MQTT */
  if(state == STATE_WAITING_FOR_MQTT_CONNECTED){
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
      }
      else{
        state = STATE_WAITING_FOR_MQTT_CONNECTED;
      }
    }
  }
}

