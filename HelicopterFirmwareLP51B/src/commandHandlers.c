#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commandHandlers.h"
#include "util.h"

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

