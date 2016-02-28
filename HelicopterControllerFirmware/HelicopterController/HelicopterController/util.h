/*
Name: util.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"
#include "pidControl.h"

#define NEWLINE					"\r\n"

void sendNack(void);
void sendAck(void);
void sendInt(int num);
void sendDouble(double num);
void sendOnOffStatus(bool isOn);
void sendOneOrZeroStatus(bool isHigh);
void sendDirectionStatus(Direction direction);
void sendMotorDriverStatus(MotorDriverType motorDriverType);
void sendIntRangeError(int lowerLimit, int upperLimit, char* unit);
void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit);
void sendChannelError(void);
void sendSyntaxError(void);
void sendReadOnlyError(void);
void sendOnOffError(void);
void sendOneOrZeroError(void);
void sendDirectionError(void);
void sendMotorDriverError(void);
void sendChannelError(void);

bool isOnCommandArg(char* arg);
bool isOffCommandArg(char* arg);
bool isClockwiseCommandArg(char* arg);
bool isCounterClockwiseCommandArg(char* arg);
bool isAnalogVoltageCommandArg(char* arg);
bool isFrequencyCommandArg(char* arg);
bool isChannelCorrect(char* channelArg);
bool isReadCommand(char* arg);
bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

void upperCaseString(char *str);
int stricmp(const char *p1, const char *p2);
int convertToInt(char *str);

#endif
