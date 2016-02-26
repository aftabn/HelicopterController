/*
Name: util.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
#include "utility.h"
#include "pidController.h"

const double Utility::DBL_ArduinoVss = 5.0;

const char* Utility::UNIT_None = "";
const char* Utility::UNIT_Percent = "%";
const char* Utility::UNIT_Milliseconds = "ms";
const char* Utility::UNIT_Hertz = "Hz";
const char* Utility::UNIT_Degrees = "°";
const char* Utility::UNIT_Volts = "V";

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

void Utility::sendError(const char* str)
{
	Serial.println(str);
	sendNack();
}

void Utility::sendInt(int num)
{
	char tmpstr[20];
	sprintf(tmpstr, "%d", num);
	Serial.println(tmpstr);
}

void Utility::sendDouble(double num)
{
	sendDouble(num, INT_DefaultNumDecimals);
}

// Using dtostrf instead of sprintf as it works better on embedded systems and doesn't
// require the full version of stdlib
// Source: http://blog.protoneer.co.nz/arduino-float-to-string-that-actually-works/
void Utility::sendDouble(double num, int numDecimals)
{
	char tmpstr[20];
	dtostrf(num, INT_MinNumFloatChars, numDecimals, tmpstr);
	Serial.println(tmpstr);
}

void Utility::sendOnOffStatus(bool isOn)
{
	char tmpstr[5];
	sprintf(tmpstr, "%s", isOn ? "ON" : "OFF");
	Serial.println(tmpstr);
}

void Utility::sendOneOrZeroStatus(bool isHigh)
{
	char tmpstr[3];
	sprintf(tmpstr, "%d", isHigh ? 1 : 0);
	Serial.println(tmpstr);
}

void Utility::sendDirectionStatus(Direction direction)
{
	char tmpstr[17];
	sprintf(tmpstr, "%s", direction == Clockwise ? "Clockwise" : "CounterClockwise");
	Serial.println(tmpstr);
}

void Utility::sendMotorDriverStatus(MotorDriverType motorDriverType)
{
	char tmpstr[17];

	if (motorDriverType == AnalogVoltage)
	{
		sprintf(tmpstr, "%s", "AnalogVoltage");
	}
	else if (motorDriverType == Frequency)
	{
		sprintf(tmpstr, "%s", "Frequency");
	}

	Serial.println(tmpstr);
}

void Utility::sendIntRangeError(int lowerLimit, int upperLimit, char* unit)
{
	char tmpstr[50];
	sprintf(tmpstr, "Value must be between %d%s and %d%s", lowerLimit, unit, upperLimit, unit);
	sendError(tmpstr);
}

void Utility::sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit)
{
	char lowerLimitStr[10];
	char upperLimitStr[10];
	char tmpstr[50];

	dtostrf(lowerLimit, INT_MinNumFloatChars, INT_DefaultNumDecimals, lowerLimitStr);
	dtostrf(upperLimit, INT_MinNumFloatChars, INT_DefaultNumDecimals, upperLimitStr);
	sprintf(tmpstr, "Value must be between %s%s and %s%s", lowerLimitStr, unit, upperLimitStr, unit);

	sendError(tmpstr);
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
	Serial.println(F("Value must be CW or CCW"));
	sendNack();
}

void Utility::sendMotorDriverError()
{
	Serial.println(F("Value must be ANALOG or FREQUENCY"));
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
	return (0 == stricmp(arg, "CW") || 0 == stricmp(arg, "0"));
}

bool Utility::isCounterClockwiseCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "CCW") || 0 == stricmp(arg, "1"));
}

bool Utility::isAnalogVoltageCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "ANALOG") || 0 == stricmp(arg, "0"));
}

bool Utility::isFrequencyCommandArg(char* arg)
{
	upperCaseString(arg);
	return (0 == stricmp(arg, "FREQUENCY") || 0 == stricmp(arg, "1"));
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

void Utility::convertDoubleToString(double num, char *str)
{
	dtostrf(num, INT_MinNumFloatChars, INT_DefaultNumDecimals, str);
}

int Utility::convertToInt(char *str)
{
	char *ptr;
	long num;

	num = strtol(str, &ptr, 10);
	return (int)num;
}