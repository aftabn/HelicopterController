/*
Name: util.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"
#include "pidControl.h"

static class Utility
{
private:
	static const int INT_DefaultNumDecimals = 2;
	static const int INT_MinNumFloatChars = 4;

public:
	static void sendNack(void);
	static void sendAck(void);
	static void sendError(const char* str);
	static void sendInt(int num);
	static void sendDouble(double num);
	static void sendDouble(double num, int numDecimals);
	static void sendOnOffStatus(bool isOn);
	static void sendOneOrZeroStatus(bool isHigh);
	static void sendDirectionStatus(Direction direction);
	static void sendMotorDriverStatus(MotorDriverType motorDriverType);
	static void sendIntRangeError(int lowerLimit, int upperLimit, char* unit);
	static void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit);
	static void sendChannelError(void);
	static void sendSyntaxError(void);
	static void sendReadOnlyError(void);
	static void sendOnOffError(void);
	static void sendOneOrZeroError(void);
	static void sendDirectionError(void);
	static void sendMotorDriverError(void);
	static void sendChannelError(void);

	static bool isOnCommandArg(char* arg);
	static bool isOffCommandArg(char* arg);
	static bool isClockwiseCommandArg(char* arg);
	static bool isCounterClockwiseCommandArg(char* arg);
	static bool isAnalogVoltageCommandArg(char* arg);
	static bool isFrequencyCommandArg(char* arg);
	static bool isChannelCorrect(char* channelArg);
	static bool isReadCommand(char* arg);
	static bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
	static bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

	static void upperCaseString(char *str);
	static int stricmp(const char *p1, const char *p2);
	static void convertDoubleToString(double num, char *str);
	static int convertToInt(char *str);
};

#endif
