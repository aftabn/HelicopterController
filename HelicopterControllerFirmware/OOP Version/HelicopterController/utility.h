/*
Name: util.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _UTILITY_h
#define _UTILITY_h

#include "arduino.h"
#include "motor.h"

class Utility
{
private:
	Utility();
	static const byte INT_DefaultNumDecimals = 2;
	static const byte INT_MinNumFloatChars = 4;

public:
	static const double DBL_ArduinoVss;

	static const char* UNIT_None;
	static const char* UNIT_Percent;
	static const char* UNIT_Milliseconds;
	static const char* UNIT_Hertz;
	static const char* UNIT_Degrees;
	static const char* UNIT_Volts;

	static const char* STR_NewLine;

	// Can't use pin 13 (on-board LED) since it's tied to SPI SCK
	// Source: http://playground.arduino.cc/Code/Spi
	static const byte PIN_HeartbeatLed = 6;
	static const byte PIN_EncoderChannelA = 4;
	static const byte PIN_EncoderChannelB = 5;
	static const byte PIN_FrequencyOutput = 7;
	static const byte PIN_FrequencyDirection = 8;
	static const byte PIN_AdcChipSelect = 9;
	static const byte PIN_DacChipSelect = 10;

	static const byte INT_MaxNumChannels = 2;
	static const byte INT_LineSizeMax = 100;
	static const byte INT_ParameterCountMax = 3;
	static const byte INT_ParameterLengthMax = 20;

	static void upperCaseString(char *str);
	static int stricmp(const char *p1, const char *p2);
	static int convertToInt(char *str);
};

#endif
