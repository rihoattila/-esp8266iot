#include <ArduinoJson.h>

#include "WiFi.h"
#include "FOTA.h"
#include "IoT.h"
#include "MQTT.h"

char mqttTopicState[23] = "0000000000000000/state";
char mqttTopicSetDO[23] = "0000000000000000/setdo";
char mqttTopicGetState[26] = "0000000000000000/getstate";

#define READINTERVAL 10 * 60 * 1000

long lastReadInterval = -1 * READINTERVAL;
char msg[100];

//level
#define TRIGGER 5
#define ECHO    4

#define R 0.55D
#define H 1.9D
#define OFFSET 0.4D

double duration, distance, distanceall;
double level;
long liter;

double a, b, c;
double alfa, sqr, circlealfa, triangle, result;

//sprinkler
#define RELAY1 13
#define RELAY2 12
#define RELAY3 14

bool Relay1 = false;
bool Relay2 = false;
bool Relay3 = false;

bool timerprogram;
long timervalue;

/* Initialization */
void setup() {

  /* Initialize pins */
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  digitalWrite(TRIGGER, LOW);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);

  timerprogram = false;
  timervalue = -1;

  /* Initialize WiFi */
  wifiSetup();

  /* Initialize FOTA */
  fotaSetup(wifiHostname);

  /* Initialize MQTT */
  mqttSetup();
  mqttSetupTopic(mqttTopicState);
  mqttSetupTopic(mqttTopicSetDO);
  mqttSetupTopic(mqttTopicGetState);
  
  mqttSubscribe(mqttTopicSetDO);
  mqttSubscribe(mqttTopicGetState);

  setPublishCallback(mqttPublishReceived);
  setReconnectCallback(mqttReconnected);

}

int compare(char* a, char* b)
{
  int ret = 0;
  if(strlen(a) == strlen(b))
  {
    for(int i = 0; i < strlen(a); i++)
    {
      if(a[i] != b[i])
      {
        ret = i;
        break;
      }
    }
  }
  else
  {
    ret = -1;
  }
  return ret;
}

void mqttReconnected()
{
  mqttSubscribe(mqttTopicSetDO);
  mqttSubscribe(mqttTopicGetState);
}

void mqttPublishReceived(char* topic, char* payload, unsigned int length)
{
  if(compare(topic, mqttTopicSetDO) == 0)
  {
    StaticJsonBuffer<200> jsonBuffer;
  
    JsonObject& root = jsonBuffer.parseObject(payload);
    if(root.success())
    {
      int domask = root["domask"];
      int state = root["state"];
      int timer = root["timer"];

      if(state)
      {
        if(state != 0)
        {
          state = 1;
        }
        timerprogram = false;
      }

      
      if(timer)
      {
        if(0 < timer and timer < 30)
        {
          state = 1;
          timervalue = millis() + timer * 60 * 1000;
          timerprogram = true;
        }
        else
        {
            mqttDebug("invalid state");
        }
      }
      else
      {
        timerprogram = false;
      }
      
      digitalWrite(RELAY1, LOW);
      Relay1 = false;
      digitalWrite(RELAY2, LOW);
      Relay2 = false;
      digitalWrite(RELAY3, LOW);
      Relay3 = false;
      
      if(domask & 1)
      {
        digitalWrite(RELAY1, state);
        if(state = 0)
        {
          Relay1 = false;
        }
        else
        {
          Relay1 = true;
        }
      }
      
      if(domask & 2)
      {
        digitalWrite(RELAY2, state);
        if(state = 0)
        {
          Relay2 = false;
        }
        else
        {
          Relay2 = true;
        }
      }
      
      if(domask & 4)
      {
        digitalWrite(RELAY3, state);
        if(state = 0)
        {
          Relay3 = false;
        }
        else
        {
          Relay3 = true;
        }
      }
    }
    sendState();
  }
  else if(compare(topic, mqttTopicGetState) == 0)
  {
    readLevel();
    sendState();
  }
  else
  {
    mqttDebug("invalid topic");
  }
}

void sendState()
{
  StaticJsonBuffer<200> jsonBuffer1;
  JsonObject& root = jsonBuffer1.createObject();

  root["level"] = level;
  root["liter"] = liter;
  root["do1"] = Relay1;
  root["do2"] = Relay2;
  root["do3"] = Relay3;

  root.printTo(msg, sizeof(msg));
  mqttPublish(mqttTopicState, msg);

}

void readLevel()
{
  distanceall = 0;

  for(int i=0; i<5; i++)
  {
    
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(2);
    
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);
    
    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = (duration/2) / 29.1;

    if(1000 < distance)
    {
      while (1000 < distance)
      {
        digitalWrite(TRIGGER, LOW);  
        delayMicroseconds(2); 
        
        digitalWrite(TRIGGER, HIGH);
        delayMicroseconds(10);
        
        digitalWrite(TRIGGER, LOW);
        duration = pulseIn(ECHO, HIGH);
        distance = (duration/2) / 29.1;
        
        delay(60);
      }
    }

    distanceall += distance;
  }
  
  level = distanceall / 5 / 100 - OFFSET;

  if(level < 0)
  {
    liter = 1800;
  }
  else
  {
    if(R < level)
    {
      a = level - R;
      b = sqrt(R * R - a * a);
  
      alfa = acos(a / R) * 180 / PI;
      sqr = R * R * PI;
      circlealfa = sqr * alfa * 2 / 360;
      triangle = a * b / 2;
      result = circlealfa - triangle * 2;
  
      liter = result * H * 1000.0;
      
    }
    else if(level < R)
    {
      a = R - level;
      b = sqrt(R * R - a * a);
  
      alfa = acos(a / R) * 180 / PI;
      sqr = R * R * PI;
      circlealfa = sqr * alfa * 2 / 360;
      triangle = a * b / 2;
      result = sqr - (circlealfa - triangle * 2);
  
      liter = result * H * 1000.0;
    }
    else
    {
      liter = 900;
    }
  }

  if(1800 < liter)
  {
    liter = 1800;
  }
  else if(liter < 0)
  {
    liter = 0;
  }

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

  if (now - lastReadInterval > READINTERVAL)
  {
    if (mqttState == MQTT_STATE_WAITING_FOR_MQTT_ACTIONS)
    {
      lastReadInterval = now;
      readLevel();
      sendState();
    }
  }

  if(timerprogram)
  {
    if(timervalue < now)
    {
      timerprogram = 0;
      timervalue = -1;
      digitalWrite(RELAY1, LOW);
      Relay1 = false;
      digitalWrite(RELAY2, LOW);
      Relay2 = false;
      digitalWrite(RELAY3, LOW);
      Relay3 = false;
      sendState();
    }
  }
  
}

