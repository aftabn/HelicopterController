// controller.h

#ifndef _CONTROLLER_h
#define _CONTROLLER_h

#include "arduino.h"

void clearParameters(void);

void processCommand(char *command);
void processLine(char *str);
void processParameters(char *remainingString);

void scanUSBPort(void);

#endif
