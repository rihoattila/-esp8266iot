#include <Ticker.h>

#include "WiFi.h"
#include "FOTA.h"
#include "IoT.h"
#include "MQTT.h"

/* Pins */
const int pinLed = 13;
const int pinRelay = 12;
unsigned char pinRelayState = 0;

/* Tickers */
Ticker tickerMain;

/* LED handler */
#define LED_PERIOD 200
unsigned char ledCounter = 0;

void LedTicker() {
  unsigned char ledActive = LED_PERIOD;
  switch (mqttState) {
    case MQTT_STATE_WAITING_FOR_IP_ADDRESS:
      ledActive = LED_PERIOD - 10;
      break;
    case MQTT_STATE_WAITING_FOR_MQTT_CONNECTED:
      ledActive = LED_PERIOD >> 1;
      break;
    case MQTT_STATE_WAITING_FOR_MQTT_ACTIONS:
      ledActive = 1;
      break;
  }
  if (ledCounter < ledActive) {
    digitalWrite(pinLed, LOW);
  }
  else {
    digitalWrite(pinLed, HIGH);
  }
  if (ledCounter < (LED_PERIOD - 1)) {
    ledCounter++;
  }
  else {
    ledCounter = 0;
  }
}

/* Initialization */
void setup() {

  /* Initialize pins */
  pinMode(pinLed, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinLed, LOW);
  digitalWrite(pinRelay, LOW);

  /* Initialize tickers */
  tickerMain.attach_ms(10, LedTicker);

  /* Initialize WiFi */
  wifiSetup();

  /* Initialize FOTA */
  fotaSetup(wifiHostname);

  /* Initialize IoT */
  iotSetup(wifiMac);

  /* Initialize MQTT */
  mqttSetup();
}

/* Main cycle */
void loop() {
  unsigned long now = millis();

  /* Handle WiFi */
  wifiLoop(now);

  /* Handle FOTA */
  fotaLoop(now);

  /* Handle IoT */
  iotLoop(now);

  /* Handle MQTT */
  mqttLoop(now);
}

