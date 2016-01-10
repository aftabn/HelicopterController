//
//
//

#include "util.h"

void send(char* str)
{
	char tmpStr[128];
	sprintf(tmpStr, "%s\r\n", str);

	Serial.write(tmpStr);
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

void sendSyntaxError()
{
	sendError("Check syntax");
}

void sendReadOnlyError()
{
	sendError("Read only commands have no additional arguments");
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