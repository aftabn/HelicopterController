/*
Name: controller.cpp
Created: 1/10/2016
Author:	Aftab
*/

#include "Arduino.h"
#include <digitalWriteFast.h>
#include <SPI.h>
#include "controller.h"
#include "utility.h"
#include "commandHandler.h"

const double Controller::DBL_FirmwareVersion = 1.07;

Controller::Controller()
{
	dac = new Dac();
	adc = new Adc();
	frequencyGenerator = new FrequencyGenerator();
	potentiometer = new Potentiometer(adc);
	encoder = Encoder::getEncoder();
	yaw = new Yaw(dac, encoder);
	tilt = new Tilt(dac, frequencyGenerator, potentiometer);
	pidController = PidController::getPidController(yaw, tilt);
}

Controller::~Controller()
{
	delete dac;
	PidController::destruct();
}

void Controller::initialize()
{
	isSafetyOn = true;
	isVerboseMode = false;

	Serial.begin(115200);
	Serial.println();
	Serial.flush();

	pinModeFast(Utility::PIN_HeartbeatLed, OUTPUT);

	initializeSpi();
	adc->initialize();
	dac->initialize();
	frequencyGenerator->initialize();
	potentiometer->initialize();
	encoder->initialize();
	yaw->initialize();
	tilt->initialize();
	pidController->initialize();
}

void Controller::initializeSpi(void)
{
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

void Controller::updateHeartbeat()
{
	if (++heartbeatCounter >= 65535)
	{
		heartbeatCounter = 0;
		digitalWriteFast(Utility::PIN_HeartbeatLed, !digitalReadFast(Utility::PIN_HeartbeatLed));
	}
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
		CommandHandler::onCommandFirmwareVersion(&DBL_FirmwareVersion);
	}
	else if (0 == strcmp(command, "PID"))
	{
		CommandHandler::onCommandPidControl(pidController);
	}
	else if (0 == strcmp(command, "VERBOSE"))
	{
		CommandHandler::onCommandVerbose(&isVerboseMode);
	}
	else if (0 == strcmp(command, "SAFETY"))
	{
		CommandHandler::onCommandSafety(&isSafetyOn);
	}
	else if (0 == strcmp(command, "O"))
	{
		CommandHandler::onCommandOutput(pidController);
	}
	else if (0 == strcmp(command, "DC"))
	{
		CommandHandler::onCommandDirection(pidController);
	}
	else if (0 == strcmp(command, "DV"))
	{
		CommandHandler::onCommandMotorDriver(pidController);
	}
	else if (0 == strcmp(command, "P"))
	{
		CommandHandler::onCommandProportionalGain(pidController);
	}
	else if (0 == strcmp(command, "I"))
	{
		CommandHandler::onCommandIntegralGain(pidController);
	}
	else if (0 == strcmp(command, "D"))
	{
		CommandHandler::onCommandDerivativeGain(pidController);
	}
	else if (0 == strcmp(command, "L"))
	{
		CommandHandler::onCommandLoopInterval(pidController);
	}
	else if (0 == strcmp(command, "W"))
	{
		CommandHandler::onCommandIntegralWindup(pidController);
	}
	else if (0 == strcmp(command, "R"))
	{
		CommandHandler::onCommandIntegralWindup(pidController);
	}
	else if (0 == strcmp(command, "SP"))
	{
		CommandHandler::onCommandSetPoint(pidController);
	}
	else if (0 == strcmp(command, "A"))
	{
		CommandHandler::onCommandAngle(pidController);
	}
	else if (0 == strcmp(command, "ADC"))
	{
		CommandHandler::onCommandAdc(adc);
	}
	else if (0 == strcmp(command, "DAC"))
	{
		CommandHandler::onCommandDacVoltage(&isSafetyOn, dac);
	}
	else if (0 == strcmp(command, "F"))
	{
		CommandHandler::onCommandFrequencyOutput(&isSafetyOn, frequencyGenerator);
	}
	else if (0 == strcmp(command, "STATE"))
	{
		CommandHandler::onCommandState(&isSafetyOn, &isVerboseMode, pidController, dac, frequencyGenerator);
	}
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
	uint8_t linePointer = 0;
	char incomingChar;
	char tmpstr[100];

	while (true)
	{
		// Use this to toggle heartbeat LED when not receiving characters
		while (Serial.available() <= 0)
		{
			updateHeartbeat();

			if (pidController->isPidCalculationNeeded)
			{
				potentiometer->updateAngle();
				pidController->executePidCalculation();
				pidController->isPidCalculationNeeded = false;
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