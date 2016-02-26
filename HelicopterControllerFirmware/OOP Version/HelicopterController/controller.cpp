/*
Name: controller.cpp
Created: 1/10/2016
Author:	Aftab
*/

#include "Arduino.h"
#include <digitalWriteFast.h>
#include "controller.h"
#include "utility.h"
#include "commandHandler.h"

Controller::Controller()
{
	lineBuffer[Utility::INT_LineSizeMax];
}

void Controller::initialize()
{
	Serial.begin(115200);
	Serial.println();
	Serial.flush();

	pinModeFast(Utility::PIN_HeartbeatLed, OUTPUT);
}

void Controller::processCommand(char *command)
{
	if (0 == strcmp(command, "*IDN?"))
	{
		CommandHandler::onCommandIdentity();
	}
	else if (0 == strcmp(command, "ECHO"))
	{
		CommandHandler::onCommandEcho();
	}
	else if (0 == strcmp(command, "CHANGELOG"))
	{
		CommandHandler::onCommandChangelog();
	}
	else if (0 == strcmp(command, "VER"))
	{
		CommandHandler::onCommandVersion();
	}
	//else if (0 == strcmp(command, "PID"))
	//{
	//	onCommandPidControl();
	//}
	//else if (0 == strcmp(command, "VERBOSE"))
	//{
	//	onCommandVerbose();
	//}
	//else if (0 == strcmp(command, "SAFETY"))
	//{
	//	onCommandSafety();
	//}
	//else if (0 == strcmp(command, "O"))
	//{
	//	onCommandOutput();
	//}
	//else if (0 == strcmp(command, "DC"))
	//{
	//	onCommandDirection();
	//}
	//else if (0 == strcmp(command, "DV"))
	//{
	//	onCommandMotorDriver();
	//}
	//else if (0 == strcmp(command, "P"))
	//{
	//	onCommandProportionalGain();
	//}
	//else if (0 == strcmp(command, "I"))
	//{
	//	onCommandIntegralGain();
	//}
	//else if (0 == strcmp(command, "D"))
	//{
	//	onCommandDerivativeGain();
	//}
	//else if (0 == strcmp(command, "L"))
	//{
	//	onCommandLoopInterval();
	//}
	//else if (0 == strcmp(command, "W"))
	//{
	//	onCommandIntegralWindup();
	//}
	//else if (0 == strcmp(command, "R"))
	//{
	//	onCommandIntegralWindup();
	//}
	//else if (0 == strcmp(command, "SP"))
	//{
	//	onCommandSetPoint();
	//}
	//else if (0 == strcmp(command, "A"))
	//{
	//	onCommandAngle();
	//}
	//else if (0 == strcmp(command, "ADC"))
	//{
	//	onCommandAdc();
	//}
	//else if (0 == strcmp(command, "DAC"))
	//{
	//	onCommandDacVoltage();
	//}
	//else if (0 == strcmp(command, "F"))
	//{
	//	onCommandFrequencyOutput();
	//}
	//else if (0 == strcmp(command, "TEST"))
	//{
	//	onCommandTest();
	//}
	////else if (0 == strcmp(command, "'"))
	////{
	////	processLine(lastCommand);
	////}
	else if (0 == strcmp(command, "HELP"))
	{
		CommandHandler::onCommandHelp();
	}
	else
	{
		CommandHandler::handleCommandUnknown(command);
	}

	/*if (!(0 == strcmp(command, "'")))
	{
		sprintf(lastCommand, "%s", lineBuffer);
	}*/
}

void Controller::clearParameters()
{
	for (int i = 0; i < Utility::INT_ParameterCountMax; i++)
	{
		strcpy(CommandHandler::gParameters[i], "");
	}
}

void Controller::processParameters(char *parameterString)
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
			strcpy(CommandHandler::gParameters[i++], token);
			token = strtok(NULL, delimiter);
		}
	}
}

void Controller::processLine(char *line)
{
	char buffer[100];
	const char delimiter[] = " ";
	char *commandString = NULL;
	char *remainingString = NULL;

	strcpy(buffer, line);
	Utility::upperCaseString(buffer);

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

void Controller::scanSerialPort()
{
	char incomingChar;
	uint8_t linePointer = 0;
	char tmpstr[100];

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
				//updatePotentiometerAngle();
			}

			if (++heartBeatTimer >= 250000)
			{
				heartBeatTimer = 0;
				digitalWriteFast(Utility::PIN_HeartbeatLed, !digitalReadFast(Utility::PIN_HeartbeatLed));
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

				if (linePointer >= Utility::INT_LineSizeMax - 1)
				{
					linePointer = Utility::INT_LineSizeMax - 1;
					lineBuffer[linePointer] = 0;
				}
			}
		}
	}
}