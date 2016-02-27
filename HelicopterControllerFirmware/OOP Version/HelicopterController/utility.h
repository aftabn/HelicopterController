/*
Name: util.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _UTILITY_h
#define _UTILITY_h

#include "arduino.h"
#include "motorEnums.h"

class Utility
{
private:
	Utility() {}
	static const byte INT_DefaultNumDecimals = 2;
	static const byte INT_MinNumFloatChars = 4;

public:
	static const double DBL_ArduinoVss;
	static const double DBL_MotorIdleVoltage;

	static const char* UNIT_None;
	static const char* UNIT_Percent;
	static const char* UNIT_Milliseconds;
	static const char* UNIT_Hertz;
	static const char* UNIT_Degrees;
	static const char* UNIT_Volts;

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

	static void sendNack(void);
	static void sendAck(void);
	static void sendError(const char* str);
	static void sendInt(int num);
	static void sendDouble(double num);
	static void sendDouble(double num, int numDecimals);
	static void sendOnOffStatus(bool isOn);
	static void sendOneOrZeroStatus(bool isHigh);
	static void sendDirectionStatus(MotorEnums::Direction direction);
	static void sendMotorDriverStatus(MotorEnums::MotorDriverType motorDriverType);
	static void sendIntRangeError(int lowerLimit, int upperLimit, char* unit);
	static void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit);
	static void sendChannelError(void);
	static void sendSyntaxError(void);
	static void sendReadOnlyError(void);
	static void sendOnOffError(void);
	static void sendOneOrZeroError(void);
	static void sendDirectionError(void);
	static void sendMotorDriverError(void);

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
