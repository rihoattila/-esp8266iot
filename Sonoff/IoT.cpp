#include "IoT.h"

/* Conversion */
const char *hex = "0123456789ABCDEF";

/* EUI */
unsigned char iotEuiRaw[8];
char *iotEuiString = "0000000000000000";

/* Initialization handler */
void iotSetup(unsigned char mac[6]) {

  /* Generate EUI (both raw and string formats) */
  iotEuiRaw[0] = mac[0] ^ 0x02;
  iotEuiRaw[1] = mac[1];
  iotEuiRaw[2] = mac[2];
  iotEuiRaw[3] = 0xff;
  iotEuiRaw[4] = 0xfe;
  iotEuiRaw[5] = mac[3];
  iotEuiRaw[6] = mac[4];
  iotEuiRaw[7] = mac[5];
  for (unsigned char i = 0; i < 8; i++) {
    unsigned char j = i << 1;
    iotEuiString[j] = hex[iotEuiRaw[i] >> 4];
    iotEuiString[j + 1] = hex[iotEuiRaw[i] & 0x0f];
  }
}

/* Main cycle handler */
void iotLoop(unsigned long now) {
}

