#ifndef _MAX5479_h

#define _MAX5479_h
#include <Arduino.h>

typedef enum {
	VREG = 0x11,
	NVREG = 0x21,
	NVREG_VREG = 0x31,
	VREG_NVREG = 0x41,
} MAX5479_Commands;

class MAX5479 {
public:
	MAX5479();

	void begin(uint8_t address);
	void writeR(uint8_t *data, uint8_t length);

private:
	uint8_t _address;
};

#endif

