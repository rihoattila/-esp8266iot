#ifndef __IOT_H
#define __IOT_H

/* EUI */
extern unsigned char iotEuiRaw[8];
extern char iotEuiString[17];
extern char iotEuiShortString[7];

/* Common */
void iotSetup(unsigned char mac[6]);
void iotLoop(unsigned long now);

#endif

