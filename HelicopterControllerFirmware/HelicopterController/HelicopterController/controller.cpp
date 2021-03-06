/*
Name: controller.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include "Arduino.h"
#include <digitalWriteFast.h>
#include "globals.h"
#include "util.h"
#include "controller.h"
#include "commandHandlers.h"

//char lastCommand[INT_LINE_SIZE_MAX + 1];
char lineBuffer[INT_LINE_SIZE_MAX + 1];

void initializeController()
{
	// The baud rate had to be dropped from 115200 to 19200 as going any higher resulted in data loss
	// due to the firing of the external interrupt for the decoder
	Serial.begin(19200);
	Serial.println();
	Serial.flush();

	pinModeFast(HEARTBEAT_LED_PIN, OUTPUT);
}

void processCommand(char *command)
{
	if (0 == strcmp(command, "O"))
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
	else if (0 == strcmp(command, "PID"))
	{
		onCommandPidControl();
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
	else if (0 == strcmp(command, "LI"))
	{
		onCommandLoopInterval();
	}
	else if (0 == strcmp(command, "IW"))
	{
		onCommandIntegralWindup();
	}
	else if (0 == strcmp(command, "RL"))
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
	else if (0 == strcmp(command, "ZE"))
	{
		onCommandZeroEncoderAngle();
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
	else if (0 == strcmp(command, "TOO"))
	{
		onCommandTiltOutputOffset();
	}
	else if (0 == strcmp(command, "ZP"))
	{
		onCommandZeroPotentiometer();
	}
	else if (0 == strcmp(command, "MF")) // Max frequency
	{
		onCommandSetMaxFrequency();
	}
	else if (0 == strcmp(command, "*IDN?"))
	{
		onCommandIdentity();
	}
	else if (0 == strcmp(command, "PING"))
	{
		onCommandPing();
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
	else if (0 == strcmp(command, "VERBOSE"))
	{
		onCommandVerbose();
	}
	else if (0 == strcmp(command, "SAFETY"))
	{
		onCommandSafety();
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
		uint8_t i = 0;
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
	char tmpstr[100];

	uint16_t heartBeatTimer = 0;
	uint16_t potentiometerTimer = 0;

	while (true)
	{
		// Use this to toggle heartbeat LED when not receiving characters
		while (Serial.available() <= 0)
		{
			if (++heartBeatTimer >= 65535)
			{
				heartBeatTimer = 0;
				digitalWriteFast(HEARTBEAT_LED_PIN, !digitalReadFast(HEARTBEAT_LED_PIN));
			}

			// Update the potentiometer only if PID isn't on, since it will be updated in the block below
			if (++potentiometerTimer >= 5000)
			{
				if (!isPidEnabled)
				{
					updatePotentiometerAngle();
				}
				potentiometerTimer = 0;
			}

			if (isPidCalculationNeeded)
			{
				updatePotentiometerAngle();
				executePidCalculation();
				isPidCalculationNeeded = false;
			}
		}

		incomingChar = (char)Serial.read();

		if (incomingChar)
		{
			if (incomingChar == '\r') // End of input
			{
				lineBuffer[linePointer] = 0;
				linePointer = 0;

				sprintf(tmpstr, "> %s", lineBuffer);
				Serial.println(tmpstr);
				processLine(lineBuffer);
			}
			else if (incomingChar == '\n') // Discard the linefeed
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