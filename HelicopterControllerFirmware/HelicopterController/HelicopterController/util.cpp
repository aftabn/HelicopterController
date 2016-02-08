/*
Name: util.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
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

void sendError(const char* str)
{
	Serial.println(str);
	sendNack();
}

void sendInt(int num)
{
	char tmpstr[20];
	sprintf(tmpstr, "%d", num);
	Serial.println(tmpstr);
}

// Using dtostrf instead of sprintf as it works better on embedded systems and doesn't
// require the full version of stdlib
// Source: http://blog.protoneer.co.nz/arduino-float-to-string-that-actually-works/
void sendDouble(double num, int numDecimals)
{
	char tmpstr[20];
	dtostrf(num, MIN_NUMBER_FLOAT_CHARS, numDecimals, tmpstr);
	Serial.println(tmpstr);
}

void sendOnOffStatus(bool isOn)
{
	if (isOn)
	{
		Serial.println(F("ON"));
	}
	else
	{
		Serial.println(F("OFF"));
	}
}

void sendOneOrZeroStatus(bool isHigh)
{
	char tmpstr[3];
	sprintf(tmpstr, "%d", isHigh ? 1 : 0);
	Serial.println(tmpstr);
}

void sendDirectionStatus(Direction direction)
{
	if (direction == Clockwise)
	{
		Serial.println(F("Clockwise"));
	}
	else if (direction == CounterClockwise)
	{
		Serial.println(F("CounterClockwise"));
	}
}

void sendMotorDriverStatus(MotorDriverType motorDriverType)
{
	if (motorDriverType == AnalogVoltage)
	{
		Serial.println(F("AnalogVoltage"));
	}
	else if (motorDriverType == Frequency)
	{
		Serial.println(F("Frequency"));
	}
}

void sendIntRangeError(int lowerLimit, int upperLimit, char* unit)
{
	char tmpstr[50];
	sprintf(tmpstr, "Value must be between %d%s and %d%s", lowerLimit, unit, upperLimit, unit);
	sendError(tmpstr);
}

void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit)
{
	char lowerLimitStr[10];
	char upperLimitStr[10];
	char tmpstr[50];

	dtostrf(lowerLimit, MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, lowerLimitStr);
	dtostrf(upperLimit, MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, upperLimitStr);
	sprintf(tmpstr, "Value must be between %s%s and %s%s", lowerLimitStr, unit, upperLimitStr, unit);

	sendError(tmpstr);
}

void sendChannelError()
{
	Serial.println(F("Invalid channel"));
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
	Serial.println(F("Value must be CW or CCW"));
	sendNack();
}

void sendMotorDriverError()
{
	Serial.println(F("Value must be ANALOGVOLTAGE or FREQUENCY"));
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
	return (0 == stricmp(arg, "CLOCKWISE") || 0 == stricmp(arg, "CW") || 0 == stricmp(arg, "0"));
}

bool isCounterClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "COUNTERCLOCKWISE") || 0 == stricmp(arg, "CCW") || 0 == stricmp(arg, "1"));
}

bool isAnalogVoltageCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ANALOGVOLTAGE") || 0 == stricmp(arg, "0"));
}

bool isFrequencyCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "FREQUENCY") || 0 == stricmp(arg, "1"));
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
	char channelStr[3];

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