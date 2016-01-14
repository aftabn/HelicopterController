//
//
//

#include "globals.h"
#include "util.h"

void send(const char* str)
{
	char tmpStr[256];
	sprintf(tmpStr, "%s\r\n", str);

	Serial.print(tmpStr);
}

void sendNack()
{
	send("ERROR");
}

void sendAck()
{
	send("OK");
}

void sendError(char* str)
{
	send(str);
	sendNack();
}

void sendInt(int num)
{
	char tmpstr[20];
	sprintf(tmpstr, "%d", num);
	send(tmpstr);
}

// Using dtostrf instead of sprintf as it works better on embedded systems and doesn't
// require the full version of stdlib
// Source: http://blog.protoneer.co.nz/arduino-float-to-string-that-actually-works/

void sendDouble(double num, int numDecimals)
{
	char tmpstr[20];
	dtostrf(num, MIN_NUMBER_FLOAT_CHARS, numDecimals, tmpstr);
	send(tmpstr);
}

void sendOnOffStatus(bool isOn)
{
	char tmpstr[5];
	sprintf(tmpstr, "%s", isOn ? "ON" : "OFF");
	send(tmpstr);
}

void sendOneOrZeroStatus(bool isHigh)
{
	char tmpstr[3];
	sprintf(tmpstr, "%d", isHigh ? 1 : 0);
	send(tmpstr);
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
	sendError("Invalid channel");
}

void sendSyntaxError()
{
	sendError("Check syntax");
}

void sendReadOnlyError()
{
	sendError("Read only commands have no additional arguments");
}

void sendOnOffError()
{
	sendError("Value must be ON or OFF");
}

void sendOneOrZeroError()
{
	sendError("Value must be 0 or 1");
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