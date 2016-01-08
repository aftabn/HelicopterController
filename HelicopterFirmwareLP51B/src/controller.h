#ifndef CONTROLLER_H_
#define CONTROLLER_H_

void clearParameters(void);

void processCommand(char *command);
void processLine(char *str);
void processParameters(char *remainingString);

void scanUSBPort(void);

#endif