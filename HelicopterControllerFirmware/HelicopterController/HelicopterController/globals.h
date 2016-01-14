// globals.h

#ifndef _GLOBALS_h
#define _GLOBALS_h

#define CONTROLLER_NAME				"Arduino Helicopter Controller"
#define FIRMWARE_VERSION			1.00

#define MAX_NUM_CHANNELS			2

#define INT_LINE_SIZE_MAX			255
#define INT_PARAMETER_COUNT_MAX		3
#define INT_PARAMETER_LENGTH_MAX	20
#define INT_DESCRIPTION_MAX			60

#define HEARTBEAT_LED				13

extern char gParameters[INT_PARAMETER_COUNT_MAX][INT_PARAMETER_LENGTH_MAX + 1];
extern int gMaximumDutyCycle;

#endif
