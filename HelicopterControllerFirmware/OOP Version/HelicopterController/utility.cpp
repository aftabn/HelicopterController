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