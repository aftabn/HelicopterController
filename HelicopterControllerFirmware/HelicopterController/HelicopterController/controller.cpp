/*
Name: controller.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include "Arduino.h"
#include "globals.h"
#include "util.h"
#include "controller.h"
#include "commandHandlers.h"

//char lastCommand[INT_LINE_SIZE_MAX + 1];
char lineBuffer[INT_LINE_SIZE_MAX + 1];

void initializeController()
{
	Serial.begin(38400);

	pinMode(HEARTBEAT_LED_PIN, OUTPUT);
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
	else if (0 == strcmp(command, "CHANGELOG"))
	{
		onCommandChangelog();
	}
	else if (0 == strcmp(command, "VER"))
	{
		onCommandVersion();
	}
	else if (0 == strcmp(command, "PID"))
	{
		onCommandPidControl();
	}
	else if (0 == strcmp(command, "VERBOSE"))
	{
		onCommandVerbose();
	}
	else if (0 == strcmp(command, "SAFETY"))
	{
		onCommandSafety();
	}
	else if (0 == strcmp(command, "O"))
	{
		onCommandOutput();
	}
	else if (0 == strcmp(command, "DC"))
	{
		onCommandDirection();
	}
	else if (0 == strcmp(command, "DV"))
	{
		onCommandMotorDriver();
	}
	else if (0 == strcmp(command, "P"))
	{
		onCommandProportionalGain();
	}
	else if (0 == strcmp(command, "I"))
	{
		onCommandIntegralGain();
	}
	else if (0 == strcmp(command, "D"))
	{
		onCommandDerivativeGain();
	}
	else if (0 == strcmp(command, "L"))
	{
		onCommandLoopInterval();
	}
	else if (0 == strcmp(command, "W"))
	{
		onCommandIntegralWindup();
	}
	else if (0 == strcmp(command, "R"))
	{
		onCommandIntegralWindup();
	}
	else if (0 == strcmp(command, "SP"))
	{
		onCommandSetPoint();
	}
	else if (0 == strcmp(command, "A"))
	{
		onCommandAngle();
	}
	else if (0 == strcmp(command, "ADC"))
	{
		onCommandAdc();
	}
	else if (0 == strcmp(command, "DAC"))
	{
		onCommandDacVoltage();
	}
	else if (0 == strcmp(command, "F"))
	{
		onCommandFrequencyOutput();
	}
	else if (0 == strcmp(command, "TEST"))
	{
		onCommandTest();
	}
	//else if (0 == strcmp(command, "'"))
	//{
	//	processLine(lastCommand);
	//}
	else if (0 == strcmp(command, "HELP"))
	{
		onCommandHelp();
	}
	else
	{
		handleCommandUnknown(command);
	}

	//if (!(0 == strcmp(command, "'")))
	//{
	//	sprintf(lastCommand, "%s", lineBuffer);
	//}
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

void scanSerialPort()
{
	char incomingChar;
	uint8_t linePointer = 0;
	char tmpstr[256];

	long heartBeatTimer = 0;
	long refreshAngleTimer = 0;

	while (true)
	{
		// Use this to toggle heartbeat LED when not receiving characters
		while (Serial.available() <= 0)
		{
			if (++refreshAngleTimer >= 5000)
			{
				refreshAngleTimer = 0;
				updatePotentiometerAngle();
			}

			if (++heartBeatTimer >= 5000)
			{
				heartBeatTimer = 0;
				digitalWrite(HEARTBEAT_LED_PIN, !digitalRead(HEARTBEAT_LED_PIN));
			}
		}

		incomingChar = (char)Serial.read();

		if (incomingChar)
		{
			if (incomingChar == '\n') // End of input
			{
				lineBuffer[linePointer] = 0;
				linePointer = 0;

				sprintf(tmpstr, "> %s", lineBuffer);
				Serial.println(tmpstr);
				processLine(lineBuffer);
			}
			else if (incomingChar == '\r') // Discard the carriage return
			{
			}
			else // Store any other characters in the buffer
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