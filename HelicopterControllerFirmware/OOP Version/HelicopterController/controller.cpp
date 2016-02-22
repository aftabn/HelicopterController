/*
Name: controller.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include "Arduino.h"
#include "globals.h"
#include "utility.h"
#include "controller.h"
#include "commandHandlers.h"

char lineBuffer[INT_LINE_SIZE_MAX + 1];

void initializeController()
{
	Serial.begin(115200);

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
	else if (0 == strcmp(command, "SYS"))
	{
		onCommandSystem();
	}
	else if (0 == strcmp(command, "PID"))
	{
		onCommandPidControl();
	}
	else if (0 == strcmp(command, "DEBUG"))
	{
		onCommandDebug();
	}
	else if (0 == strcmp(command, "SAFETY"))
	{
		onCommandSafety();
	}
	else if (0 == strcmp(command, "OUT"))
	{
		onCommandOutput();
	}
	else if (0 == strcmp(command, "DIR"))
	{
		onCommandDirection();
	}
	else if (0 == strcmp(command, "DRIVER"))
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
	else if (0 == strcmp(command, "LOOP"))
	{
		onCommandLoopInterval();
	}
	else if (0 == strcmp(command, "SP"))
	{
		onCommandSetPoint();
	}
	else if (0 == strcmp(command, "ANGLE"))
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
	else if (0 == strcmp(command, "FREQ"))
	{
		onCommandFrequencyOutput();
	}
	else if (0 == strcmp(command, "STATE"))
	{
		onCommandState();
	}
	else if (0 == strcmp(command, "TEST"))
	{
		onCommandTest();
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

			if (++heartBeatTimer >= 250000)
			{
				heartBeatTimer = 0;
				digitalWrite(HEARTBEAT_LED_PIN, !digitalRead(HEARTBEAT_LED_PIN));
			}
		}

		incomingChar = (char)Serial.read();

		if (incomingChar)
		{
			if (incomingChar == '\n')  // 13 = \r
			{
				lineBuffer[linePointer] = 0;
				linePointer = 0;

				sprintf(tmpstr, "> %s", lineBuffer);
				Serial.println(tmpstr);
				processLine(lineBuffer);
			}
			else if (incomingChar == '\r') // 10 = \n
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