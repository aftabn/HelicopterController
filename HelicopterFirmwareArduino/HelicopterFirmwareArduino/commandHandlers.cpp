//
//
//

#include "globals.h"
#include "util.h"
#include "commandHandlers.h"

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
	if (0 == stricmp(gParameters[0], "VER"))
	{
		sendDouble(FIRMWARE_VERSION);
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