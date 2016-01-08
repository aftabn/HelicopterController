#include <stdio.h>
#include <globals.h>
#include <commandHandlers.h>
#include <string.h>
#include <util.h>

void onCommandIdentity()
{
	send(CONTROLLER_NAME);
	sendAck();
}

void onCommandEcho()
{
	if (gParameters[0] != NULL)
	{
		send(gParameters[0]);
		sendAck();
	}
	else
	{
		sendNack();
	}
}

void onCommandSystem()
{
	if (0 == stricmp(gParameters[0], "SERIAL_NUMBER"))
	{
		getSerialNumber();
		sendAck();
	}
	else if (0 == stricmp(gParameters[0], "JIG_ID"))
	{
		int id = getJigId();
		sendInt(id);
		sendAck();
	}
	else
	{
		sendSyntaxError();
	}
}

void onCommandHelp()
{
	send("Command: *IDN? \r\nDescription: Returns the identity of the controller\r\n");
	send("Command: SYS \r\nArg: VER \r\nDescription: Gets information about jig or the controller\r\n");
	send("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n");
}

void handleCommandUnknown(char* command)
{
	char tmpstr[128];
	char newstr[40];
	sprintf(tmpstr, "Unknown command [%s]", command);
	
	for (int i = 0; i < INT_PARAMETER_COUNT_MAX; i++)
	{
		if (strlen(gParameters[i]))
		{
			sprintf(newstr, "  %d{%s}", i, gParameters[i]);
			strcat(tmpstr, newstr);
		}
	}
	
	send(tmpstr);
	sendNack();
}


bool isReadCommand(char* arg)
{
	return (0 == stricmp(arg, ""));
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

bool isChannelCorrect(char* channelArg)
{
	char channelStr[3];
	
	for(int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		sprintf(channelStr, "%d", channel);
		if (0 == stricmp(channelArg, channelStr))
		{
			return true;
		}
	}
	
	return false;
}

bool isIntWithinRange(int number, int lowerLimit, int upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

void send(char* str)
{
	printf("%s\r\n", str);
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

int stricmp (const char *p1, const char *p2)
{
	register unsigned char *s1 = (unsigned char *) p1;
	register unsigned char *s2 = (unsigned char *) p2;
	unsigned char c1, c2;
	
	do
	{
		c1 = (unsigned char) toupper((int)*s1++);
		c2 = (unsigned char) toupper((int)*s2++);
		if (c1 == '\0')
		{
			return c1 - c2;
		}
	}
	while (c1 == c2);
	
	return c1 - c2;
}