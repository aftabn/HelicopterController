/*
Name: util.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"

#define DEFAULT_NUM_DECIMALS	2
#define MIN_NUMBER_FLOAT_CHARS	4

void send(const char *str);
void sendNack(void);
void sendAck(void);
void sendError(char* str);
void sendInt(int num);
void sendDouble(double num, int numDecimals);
void sendOnOffStatus(bool isOn);
void sendOneOrZeroStatus(bool isHigh);
void sendIntRangeError(int lowerLimit, int upperLimit, char* unit);
void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit);
void sendChannelError(void);
void sendSyntaxError(void);
void sendReadOnlyError(void);
void sendOnOffError(void);
void sendOneOrZeroError(void);
void sendChannelError();

bool isOnCommandArg(char* arg);
bool isOffCommandArg(char* arg);
bool isChannelCorrect(char* channelArg);
bool isReadCommand(char* arg);
bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

void upperCaseString(char *str);
int stricmp(const char *p1, const char *p2);
int convertToInt(char *str);

#endif
