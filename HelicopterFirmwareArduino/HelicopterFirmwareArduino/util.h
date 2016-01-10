// util.h

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"

void send(char *str);
void sendNack(void);
void sendAck(void);
void sendError(char* str);
void sendInt(int num);
void sendDouble(double num);
void sendSyntaxError(void);
void sendReadOnlyError(void);

bool isReadCommand(char* arg);
bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

void upperCaseString(char *str);
int stricmp(const char *p1, const char *p2);

#endif
