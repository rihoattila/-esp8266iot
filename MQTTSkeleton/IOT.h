#include <ESP8266WiFi.h>

const char * hex = "0123456789ABCDEF";
char * EUID = "0000000000000000";

void createEUID(){
  byte mqttClientIdRaw[8];
  byte wifiMac[6];
  
  WiFi.macAddress(wifiMac);
  
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
    EUID[j] = hex[mqttClientIdRaw[i] >> 4];
    EUID[j + 1] = hex[mqttClientIdRaw[i] & 0x0f];
  }
}
