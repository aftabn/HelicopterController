// globals.h

#ifndef _GLOBALS_h
#define _GLOBALS_h

#include "arduino.h"

#define CONTROLLER_NAME				"Arduino Helicopter Controller"
#define FIRMWARE_VERSION			1.00

#define INT_LINE_SIZE_MAX			256
#define INT_PARAMETER_COUNT_MAX		3
#define INT_PARAMETER_LENGTH_MAX	40
#define INT_DESCRIPTION_MAX			60

extern char lineBuffer[INT_LINE_SIZE_MAX + 1];
extern char gParameters[INT_PARAMETER_COUNT_MAX][INT_PARAMETER_LENGTH_MAX + 1];

#endif
