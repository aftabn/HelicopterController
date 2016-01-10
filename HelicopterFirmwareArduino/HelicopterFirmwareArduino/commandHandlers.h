// commandHandlers.h

#ifndef _COMMANDHANDLERS_h
#define _COMMANDHANDLERS_h

#include "arduino.h"

void onCommandEcho(void);
void onCommandIdentity(void);
void onCommandSystem(void);
void onCommandTest(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

#endif
