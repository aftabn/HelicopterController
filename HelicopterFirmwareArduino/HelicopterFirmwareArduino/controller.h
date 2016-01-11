// controller.h

#ifndef _CONTROLLER_h
#define _CONTROLLER_h

void initializeController(void);
void toggleHeartbeatLED(void);

void clearParameters(void);

void processCommand(char *command);
void processLine(char *str);
void processParameters(char *remainingString);

void scanUSBPort(void);

#endif
