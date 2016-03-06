/*
Name: util.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
#include <Streaming.h>
#include "globals.h"
#include "util.h"

// Implementation of sending had to change due to arduino storing string literals in dynamic memory
// Used F() to force storage of literals in PROGMEM, which requires directly using Serial.println() to send
// This is because Serial.print has the proper overload to handle [const PROGMEM char *] conversion
// Source: https://www.arduino.cc/en/Reference/PROGMEM

void sendNack()
{
	Serial.println(F("ERROR"));
}

void sendAck()
{
	Serial.println(F("OK"));
}

void sendInt(int num)
{
	Serial << num << NEWLINE;
}

void sendDouble(double num)
{
	Serial << num << NEWLINE;
}

void sendOnOffStatus(bool isOn)
{
	Serial << (isOn ? F("ON") : F("OFF")) << NEWLINE;
}

void sendOneOrZeroStatus(bool isHigh)
{
	Serial << (isHigh ? 1 : 0) << NEWLINE;
}

void sendDirectionStatus(Direction direction)
{
	Serial << (direction == Clockwise ? F("CW") : F("CCW")) << NEWLINE;
}

void sendMotorDriverStatus(MotorDriverType motorDriverType)
{
	Serial << (motorDriverType == AnalogVoltage ? F("AV") : F("F")) << NEWLINE;
}

void sendIntRangeError(int lowerLimit, int upperLimit, char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << NEWLINE;
	sendNack();
}

void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << NEWLINE;
	sendNack();
}

void sendChannelError()
{
	Serial.println(F("Invalid channel"));
	sendNack();
}

void sendProfileError()
{
	Serial.println(F("Invalid profile"));
	sendNack();
}

void sendSyntaxError()
{
	Serial.println(F("Check syntax"));
	sendNack();
}

void sendReadOnlyError()
{
	Serial.println(F("Read only commands have no additional arguments"));
	sendNack();
}

void sendOnOffError()
{
	Serial.println(F("Value must be ON or OFF"));
	sendNack();
}

void sendOneOrZeroError()
{
	Serial.println(F("Value must be 0 or 1"));
	sendNack();
}

void sendDirectionError()
{
	Serial.println(F("Value must be CLOCKWISE / CW or COUNTERCLOCKWISE / CCW"));
	sendNack();
}

void sendMotorDriverError()
{
	Serial.println(F("Value must be ANALOGVOLTAGE / AV or FREQUENCY / F"));
	sendNack();
}

bool isOnCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ON") || 0 == stricmp(arg, "1"));
}

bool isOffCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "OFF") || 0 == stricmp(arg, "0"));
}

bool isClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "CLOCKWISE") || 0 == stricmp(arg, "CW"));
}

bool isCounterClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "COUNTERCLOCKWISE") || 0 == stricmp(arg, "CCW"));
}

bool isAnalogVoltageCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ANALOGVOLTAGE") || 0 == stricmp(arg, "AV"));
}

bool isFrequencyCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "FREQUENCY") || 0 == stricmp(arg, "F"));
}

bool isReadCommand(char* arg)
{
	return (0 == stricmp(arg, ""));
}

bool isIntWithinRange(int number, int lowerLimit, int upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool isChannelCorrect(char* channelArg)
{
	char channelStr[10];

	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		sprintf(channelStr, "%d", channel);
		if (0 == stricmp(channelArg, channelStr))
		{
			return true;
		}
	}

	return false;
}

bool isProfileCorrect(char* profileArg)
{
	char profileStr[10];

	for (int profile = 0; profile < NUM_DIRECTION_PROFILES; profile++)
	{
		sprintf(profileStr, "%d", profileStr);
		if (0 == stricmp(profileArg, profileStr))
		{
			return true;
		}
	}

	return false;
}

void upperCaseString(char *str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		str[i] = toupper(str[i]);
	}
}

int stricmp(const char *p1, const char *p2)
{
	register unsigned char *s1 = (unsigned char *)p1;
	register unsigned char *s2 = (unsigned char *)p2;
	unsigned char c1, c2;

	do
	{
		c1 = (unsigned char)toupper((int)*s1++);
		c2 = (unsigned char)toupper((int)*s2++);
		if (c1 == '\0')
		{
			return c1 - c2;
		}
	} while (c1 == c2);

	return c1 - c2;
}

int convertToInt(char *str)
{
	char *ptr;
	long num;

	num = strtol(str, &ptr, 10);
	return (int)num;
}