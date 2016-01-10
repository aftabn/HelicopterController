//
//
//

#include "Arduino.h"
#include "globals.h"
#include "util.h"
#include "controller.h"
#include "commandHandlers.h"

void initController()
{
	Serial.begin(115200);
	pinMode(HEARTBEAT_LED, OUTPUT);
}

void toggleHeartbeatLED()
{
	digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED));
}

void processCommand(char *command)
{
	if (0 == strcmp(command, "*IDN?"))
	{
		onCommandIdentity();
	}
	else if (0 == strcmp(command, "ECHO"))
	{
		onCommandEcho();
	}
	else if (0 == strcmp(command, "SYS"))
	{
		onCommandSystem();
	}
	else if (0 == strcmp(command, "HELP"))
	{
		onCommandHelp();
	}
	else
	{
		handleCommandUnknown(command);
	}
}

void clearParameters()
{
	for (int i = 0; i < INT_PARAMETER_COUNT_MAX; i++)
	{
		strcpy(gParameters[i], "");
	}
}

void processParameters(char *parameterString)
{
	clearParameters();

	if (NULL != parameterString && strlen(parameterString))
	{
		int i = 0;
		const char delimiter[] = " ";
		char *token;

		// break up remaining strings into parameters
		token = strtok(parameterString, delimiter);

		while (NULL != token)
		{
			// copy parameters into the global parameter variables
			strcpy(gParameters[i++], token);
			token = strtok(NULL, delimiter);
		}
	}
}

void processLine(char *line)
{
	char buffer[256];
	const char delimiter[] = " ";
	char *commandString = NULL;
	char *remainingString = NULL;

	strcpy(buffer, line);
	upperCaseString(buffer);

	// get first token
	commandString = strtok(buffer, delimiter);

	if (NULL != commandString)
	{
		if (strlen(line) - strlen(commandString) > 0)
		{
			remainingString = line + strlen(commandString) + 1;
		}

		processParameters(remainingString);
		processCommand(commandString);
	}
}

void scanUSBPort()
{
	char incomingChar;
	uint16_t linePointer = 0;
	char tmpstr[256];

	long heartBeatTimer = 0;

	while (true)
	{
		// Use this to toggle heartbeat LED when not receiving characters
		while (Serial.available() <= 0)
		{
			if (++heartBeatTimer >= 250000)
			{
				heartBeatTimer = 0;
				toggleHeartbeatLED();
			}
		}

		incomingChar = Serial.read();

		if (incomingChar)
		{
			if (incomingChar == 13)  // \r
			{
				lineBuffer[linePointer] = 0;
				linePointer = 0;

				sprintf(tmpstr, "> %s", lineBuffer);
				send(tmpstr);
				processLine(lineBuffer);
			}
			else if (incomingChar == 10) // \n
			{
			}
			else
			{
				lineBuffer[linePointer++] = incomingChar;
				lineBuffer[linePointer] = 0;

				if (linePointer >= INT_LINE_SIZE_MAX - 1)
				{
					linePointer = INT_LINE_SIZE_MAX - 1;
					lineBuffer[linePointer] = 0;
				}
			}
		}
	}
}