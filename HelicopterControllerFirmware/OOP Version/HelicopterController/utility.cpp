/*
Name: util.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
#include <Streaming.h>
#include "motor.h"
#include "utility.h"

using namespace Motor;

const double Utility::DBL_ArduinoVss = 5.0;

const char* Utility::UNIT_None = "";
const char* Utility::UNIT_Percent = "%";
const char* Utility::UNIT_Milliseconds = "ms";
const char* Utility::UNIT_Hertz = "Hz";
const char* Utility::UNIT_Degrees = "deg";
const char* Utility::UNIT_Volts = "V";

const char* Utility::STR_NewLine = "\r\n";

Utility::Utility() {}

// Implementation of sending had to change due to arduino storing string literals in dynamic memory
// Used F() to force storage of literals in PROGMEM, which requires directly using Serial.println() to send
// This is because Serial.print has the proper overload to handle [const PROGMEM char *] conversion
// Source: https://www.arduino.cc/en/Reference/PROGMEM

void Utility::sendNack()
{
	Serial.println(F("ERROR"));
}

void Utility::sendAck()
{
	Serial.println(F("OK"));
}

void Utility::sendInt(int num)
{
	Serial << num << STR_NewLine;
}

void Utility::sendDouble(double num)
{
	Serial << num << STR_NewLine;
}

void Utility::sendOnOffStatus(bool isOn)
{
	Serial << (isOn ? F("ON") : F("OFF")) << STR_NewLine;
}

void Utility::sendOneOrZeroStatus(bool isHigh)
{
	Serial << (isHigh ? 1 : 0) << STR_NewLine;
}

void Utility::sendDirectionStatus(Direction direction)
{
	Serial << (direction == Direction::Clockwise ? F("CW") : F("CCW")) << STR_NewLine;
}

void Utility::sendMotorDriverStatus(MotorDriverType motorDriverType)
{
	Serial << (motorDriverType == MotorDriverType::AnalogVoltage ? F("AV") : F("F")) << STR_NewLine;
}

void Utility::sendIntRangeError(int lowerLimit, int upperLimit, char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << STR_NewLine;
	sendNack();
}

void Utility::sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << STR_NewLine;
	sendNack();
}

void Utility::sendChannelError()
{
	Serial.println(F("Invalid channel"));
	sendNack();
}

void Utility::sendSyntaxError()
{
	Serial.println(F("Check syntax"));
	sendNack();
}

void Utility::sendReadOnlyError()
{
	Serial.println(F("Read only commands have no additional arguments"));
	sendNack();
}

void Utility::sendOnOffError()
{
	Serial.println(F("Value must be ON or OFF"));
	sendNack();
}

void Utility::sendOneOrZeroError()
{
	Serial.println(F("Value must be 0 or 1"));
	sendNack();
}

void Utility::sendDirectionError()
{
	Serial.println(F("Value must be CLOCKWISE / CW or COUNTERCLOCKWISE / CCW"));
	sendNack();
}

void Utility::sendMotorDriverError()
{
	Serial.println(F("Value must be ANALOGVOLTAGE / AV or FREQUENCY / F"));
	sendNack();
}

bool Utility::isOnCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ON") || 0 == stricmp(arg, "1"));
}

bool Utility::isOffCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "OFF") || 0 == stricmp(arg, "0"));
}

bool Utility::isClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "CLOCKWISE") || 0 == stricmp(arg, "CW"));
}

bool Utility::isCounterClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "COUNTERCLOCKWISE") || 0 == stricmp(arg, "CCW"));
}

bool Utility::isAnalogVoltageCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ANALOGVOLTAGE") || 0 == stricmp(arg, "AV"));
}

bool Utility::isFrequencyCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "FREQUENCY") || 0 == stricmp(arg, "F"));
}

bool Utility::isReadCommand(char* arg)
{
	return (0 == stricmp(arg, ""));
}

bool Utility::isIntWithinRange(int number, int lowerLimit, int upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool Utility::isDoubleWithinRange(double number, double lowerLimit, double upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool Utility::isChannelCorrect(char* channelArg)
{
	char channelStr[3];

	for (int channel = 0; channel < INT_MaxNumChannels; channel++)
	{
		sprintf(channelStr, "%d", channel);
		if (0 == stricmp(channelArg, channelStr))
		{
			return true;
		}
	}

	return false;
}

void Utility::upperCaseString(char *str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		str[i] = toupper(str[i]);
	}
}

int Utility::stricmp(const char *p1, const char *p2)
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

int Utility::convertToInt(char *str)
{
	char *ptr;
	long num;

	num = strtol(str, &ptr, 10);
	return (int)num;
}