/*
Name: controller.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _CONTROLLER_h
#define _CONTROLLER_h

void initializeController(void);
void toggleHeartbeatLED(void);

void clearParameters(void);

void processCommand(char *command);
void processLine(char *str);
void processParameters(char *remainingString);

void scanSerialPort(void);

#endif
