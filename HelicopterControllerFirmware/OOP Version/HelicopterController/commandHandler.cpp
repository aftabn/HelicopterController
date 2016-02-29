/*
Name: commandHandler.cpp
Created: 1/10/2016
Author:	Aftab
*/

#include <Streaming.h>
#include "motor.h"
#include "commandHandler.h"
#include "pidController.h"

using namespace Motor;

char CommandHandler::gParameters[Utility::INT_ParameterCountMax][Utility::INT_ParameterLengthMax + 1];

void CommandHandler::onCommandIdentity()
{
	Serial.println(F("Arduino Helicopter Controller"));
	sendAck();
}

void CommandHandler::onCommandEcho()
{
	if (gParameters[0] != NULL)
	{
		Serial.println(gParameters[0]);
		sendAck();
	}
	else
	{
		sendNack();
	}
}

void CommandHandler::onCommandChangelog()
{
	if (isReadCommand(gParameters[0]))
	{
		Serial.println(F("Fixed outputs not updating from disabling PID"));
		sendAck();
	}
	else
	{
		sendReadOnlyError();
	}
}

void CommandHandler::onCommandFirmwareVersion(const double *firmwareVersion)
{
	if (isReadCommand(gParameters[0]))
	{
		sendDouble(*firmwareVersion);
		sendAck();
	}
	else
	{
		sendReadOnlyError();
	}
}

void CommandHandler::onCommandPidControl(PidController *pidController)
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(pidController->isPidEnabled);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		if (!pidController->isPidEnabled)
		{
			pidController->enablePid();
			sendAck();
		}
		else
		{
			Serial.println(F("PID control is already on."));
			sendNack();
		}
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		pidController->disablePid();
		sendAck();
	}
	else
	{
		sendOnOffError();
	}
}

void CommandHandler::onCommandVerbose(bool *isVerboseMode)
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(*isVerboseMode);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		*isVerboseMode = true;
		sendAck();
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		*isVerboseMode = false;
		sendAck();
	}
	else

		sendOnOffError();
}

void CommandHandler::onCommandSafety(bool *isSafetyOn)
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(*isSafetyOn);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		*isSafetyOn = true;
		sendAck();
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		*isSafetyOn = false;
		sendAck();
	}
	else
	{
		sendOnOffError();
	}
}

void CommandHandler::onCommandOutput(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		int output = Utility::convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(*(pidController->currentOutputs[channel]));
			sendAck();
		}
		else if (!pidController->isPidEnabled)
		{
			if (isIntWithinRange(output, pidController->INT_MinPidOutput, pidController->INT_MaxPidOutput))
			{
				Direction direction = *(pidController->directions[channel]);

				pidController->applyMotorOutputs(channel, &direction, &output);
				sendAck();
			}
			else
			{
				sendIntRangeError(pidController->INT_MinPidOutput, pidController->INT_MaxPidOutput, Utility::UNIT_Percent);
			}
		}
		else
		{
			Serial.println(F("Cannot change percentage output while PID control is on."));
			sendNack();
		}
	}
	else
	{
		sendChannelError();
	}
}

// Allows the reading or changing of direction for specified motor, but only allows change in
// direction if the change results in a percentage change of less than the outputRateLimit
void CommandHandler::onCommandDirection(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDirectionStatus(*(pidController->directions[channel]));
			sendAck();
		}
		else if (!pidController->isPidEnabled)
		{
			int output = *(pidController->currentOutputs[channel]);
			int outputRateLimit = *(pidController->outputRateLimits[channel]);
			Direction direction;

			if (isClockwiseCommandArg(gParameters[1]))
			{
				direction = Direction::Clockwise;
			}
			else if (isCounterClockwiseCommandArg(gParameters[1]))
			{
				direction = Direction::CounterClockwise;
			}
			else
			{
				sendDirectionError();
				return;
			}

			if (direction != *(pidController->directions[channel]) && output > outputRateLimit / 2)
			{
				Serial << F("Cannot make a change to output by more than ") << outputRateLimit << F("%") << Utility::STR_NewLine;
				sendNack();
			}
			else
			{
				pidController->applyMotorOutputs(channel, &direction, &output);
				sendAck();
			}
		}
		else
		{
			Serial.println(F("Cannot change direction while PID control is on."));
			sendNack();
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandMotorDriver(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendMotorDriverStatus(*(pidController->motorDriverTypes[channel]));
			sendAck();
		}
		else if (!pidController->isPidEnabled)
		{
			if (isAnalogVoltageCommandArg(gParameters[1]))
			{
				*(pidController->motorDriverTypes[channel]) = MotorDriverType::AnalogVoltage;
				sendAck();
			}
			else if (isFrequencyCommandArg(gParameters[1]))
			{
				*(pidController->motorDriverTypes[channel]) = MotorDriverType::Frequency;
				sendAck();
			}
			else
			{
				sendMotorDriverError();
			}
		}
		else
		{
			Serial.println(F("Cannot change driver type while PID control is on."));
			sendNack();
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandProportionalGain(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double pGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->proportionalGains[channel]));
			sendAck();
		}
		else if (isDoubleWithinRange(pGain, PidSettings::DBL_ProportionalGainMin, PidSettings::DBL_ProportionalGainMax))
		{
			*(pidController->proportionalGains[channel]) = pGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(PidSettings::DBL_ProportionalGainMin, PidSettings::DBL_ProportionalGainMax, Utility::UNIT_None);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandIntegralGain(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double iGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->integralGains[channel]));
			sendAck();
		}
		else if (isDoubleWithinRange(iGain, PidSettings::DBL_IntegralGainMin, PidSettings::DBL_IntegralGainMax))
		{
			*(pidController->integralGains[channel]) = iGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(PidSettings::DBL_IntegralGainMin, PidSettings::DBL_IntegralGainMax, Utility::UNIT_None);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandDerivativeGain(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double dGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->derivativeGains[channel]));
			sendAck();
		}
		else if (isDoubleWithinRange(dGain, PidSettings::DBL_DerivativeGainMin, PidSettings::DBL_DerivativeGainMax))
		{
			*(pidController->derivativeGains[channel]) = dGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(PidSettings::DBL_DerivativeGainMin, PidSettings::DBL_DerivativeGainMax, Utility::UNIT_None);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandSetPoint(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double setPoint = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->setPoints[channel]));
			sendAck();
		}
		else if (isDoubleWithinRange(setPoint, *(pidController->minSetPoints[channel]), *(pidController->maxSetPoints[channel])))
		{
			*(pidController->setPoints[channel]) = setPoint;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(*(pidController->minSetPoints[channel]), *(pidController->maxSetPoints[channel]), Utility::UNIT_Degrees);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandLoopInterval(PidController *pidController)
{
	int loopInterval = Utility::convertToInt(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendInt(pidController->pidLoopInterval);
		sendAck();
	}
	else if (isIntWithinRange(loopInterval, pidController->INT_MinPidLoopInterval, pidController->INT_MaxPidLoopInterval))
	{
		if (!pidController->isPidEnabled)
		{
			pidController->pidLoopInterval = loopInterval;
			sendAck();
		}
		else
		{
			Serial.println(F("Cannot change loop interval while PID control is on."));
			sendNack();
		}
	}
	else
	{
		sendIntRangeError(pidController->INT_MinPidLoopInterval, pidController->INT_MaxPidLoopInterval, Utility::UNIT_Milliseconds);
	}
}

void CommandHandler::onCommandIntegralWindup(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double windup = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->integralWindupThresholds[channel]));
			sendAck();
		}
		else if (isDoubleWithinRange(windup, PidSettings::DBL_IntegralWindupThresholdMin, PidSettings::DBL_IntegralWindupThresholdMax))
		{
			*(pidController->integralWindupThresholds[channel]) = windup;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(PidSettings::DBL_IntegralWindupThresholdMin, PidSettings::DBL_IntegralWindupThresholdMax, Utility::UNIT_Degrees);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandRateLimit(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		int rateLimit = Utility::convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(*(pidController->outputRateLimits[channel]));
			sendAck();
		}
		else if (isIntWithinRange(rateLimit, PidSettings::INT_OutputRateLimitMin, PidSettings::INT_OutputRateLimitMax))
		{
			*(pidController->outputRateLimits[channel]) = rateLimit;
			sendAck();
		}
		else
		{
			sendIntRangeError(PidSettings::INT_OutputRateLimitMin, PidSettings::INT_OutputRateLimitMax, Utility::UNIT_Degrees);
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandAngle(PidController *pidController)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(*(pidController->currentAngles[channel]));
			sendAck();
		}
		else
		{
			sendReadOnlyError();
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandAdc(Adc *adc)
{
	int adcChannel = Utility::convertToInt(gParameters[0]);

	if (isIntWithinRange(adcChannel, adc->INT_ChannelMin, adc->INT_ChannelMax))
	{
		double voltage = adc->getSampledVoltage(adcChannel);
		sendDouble(voltage);
		sendAck();
	}
	else
	{
		sendIntRangeError(adc->INT_ChannelMin, adc->INT_ChannelMax, Utility::UNIT_None);
	}
}

void CommandHandler::onCommandDacVoltage(bool *isSafetyOn, Dac *dac)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		double voltage = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(dac->currentVoltages[channel]);
			sendAck();
		}
		else
		{
			if (!*isSafetyOn)
			{
				if (isDoubleWithinRange(voltage, Motor::DBL_MotorVoltageMin, Motor::DBL_MotorVoltageMax))
				{
					dac->setVoltage(channel, voltage);
					sendAck();
				}
				else
				{
					sendDoubleRangeError(Motor::DBL_MotorVoltageMin, Motor::DBL_MotorVoltageMax, Utility::UNIT_Volts);
				}
			}
			else
			{
				Serial.println(F("Cannot change voltage while safety is on."));
				sendNack();
			}
		}
	}
	else
	{
		sendChannelError();
	}
}

// This command is complex because it only allows setting of frequency for channel 1
// This is because only the tilt motor can be controlled by frequency
void CommandHandler::onCommandFrequencyOutput(bool *isSafetyOn, FrequencyGenerator *frequencyGenerator)
{
	if (isChannelCorrect(gParameters[0]))
	{
		byte channel = Utility::convertToInt(gParameters[0]);
		int frequency = Utility::convertToInt(gParameters[1]);

		if (channel == Tilt::INT_MotorChannel)
		{
			if (isReadCommand(gParameters[1]))
			{
				sendInt(frequencyGenerator->currentFrequency);
				sendAck();
			}
			else
			{
				if (!*isSafetyOn)
				{
					if (isIntWithinRange(frequency, Motor::INT_MotorMinFrequency, Motor::INT_MotorMaxFrequency))
					{
						frequencyGenerator->setFrequency(frequency);
						sendAck();
					}
					else
					{
						sendIntRangeError(Motor::INT_MotorMinFrequency, Motor::INT_MotorMaxFrequency, Utility::UNIT_Hertz);
					}
				}
				else
				{
					Serial.println(F("Cannot change frequency output while safety is on."));
					sendNack();
				}
			}
		}
		else
		{
			Serial.println(F("Changing or reading of frequency only applies to channel 1"));
			sendNack();
		}
	}
	else
	{
		sendChannelError();
	}
}

void CommandHandler::onCommandState(bool *isSafetyOn, bool *isVerboseMode, PidController *pidController, Dac *dac, FrequencyGenerator *frequencyGenerator)
{
	char yawDirection[4];
	char tiltDirection[4];
	char yawMotorDriver[3];
	char tiltMotorDriver[3];
	sprintf(yawDirection, *(pidController->directions[0]) == Direction::Clockwise ? "CW" : "CCW");
	sprintf(tiltDirection, *(pidController->directions[1]) == Direction::Clockwise ? "CW" : "CCW");
	sprintf(yawMotorDriver, *(pidController->motorDriverTypes[0]) == MotorDriverType::AnalogVoltage ? "AV" : "F");
	sprintf(tiltMotorDriver, *(pidController->motorDriverTypes[1]) == MotorDriverType::AnalogVoltage ? "AV" : "F");

	Serial.println(F("|===============================|"));
	Serial.println(F("| Channel\t0\t1\t|"));
	Serial.println(F("|-------------------------------|"));
	Serial << F("| Output\t") << *(pidController->currentOutputs[0]) << F("\t") << *(pidController->currentOutputs[1]) << F("\t|\r\n");
	Serial << F("| Directions\t") << yawDirection << F("\t") << tiltDirection << F("\t|\r\n");
	Serial << F("| Angles\t") << *(pidController->currentAngles[0]) << F("\t") << *(pidController->currentAngles[1]) << F("\t|\r\n");
	Serial << F("| Set Points\t") << *(pidController->setPoints[0]) << F("\t") << *(pidController->setPoints[1]) << F("\t|\r\n");
	Serial << F("| DAC\t\t") << dac->currentVoltages[0] << F("\t") << dac->currentVoltages[1] << F(" V\t|\r\n");
	Serial << F("| Frequency\t") << frequencyGenerator->currentFrequency << F(" Hz\t(CH. 1)\t|\r\n");
	Serial << F("| PID Interval\t") << pidController->pidLoopInterval << F(" ms\t\t|\r\n");
	Serial << F("| PID Control\t") << (pidController->isPidEnabled ? F("On") : F("Off")) << F("\t\t|\r\n");
	Serial << F("| P-Gain\t") << *(pidController->proportionalGains[0]) << F("\t") << *(pidController->proportionalGains[1]) << F("\t|\r\n");
	Serial << F("| I-Gain\t") << *(pidController->integralGains[0]) << F("\t") << *(pidController->integralGains[1]) << F("\t|\r\n");
	Serial << F("| D-Gain\t") << *(pidController->derivativeGains[0]) << F("\t") << *(pidController->derivativeGains[1]) << F("\t|\r\n");
	Serial << F("| I-Windup\t") << *(pidController->integralWindupThresholds[0]) << F("\t") << *(pidController->integralWindupThresholds[1]) << F("\t|\r\n");
	Serial << F("| Out R.L.\t") << *(pidController->outputRateLimits[0]) << F("\t") << *(pidController->outputRateLimits[1]) << F("\t|\r\n");
	Serial << F("| Drivers\t") << yawMotorDriver << F("\t") << tiltMotorDriver << F("\t|\r\n");
	Serial << F("| Safety\t") << (*isSafetyOn ? F("On") : F("Off")) << F("\t\t|\r\n");
	Serial << F("| Verbose\t") << (*isVerboseMode ? F("On") : F("Off")) << F("\t\t|\r\n");
	Serial.println(F("|===============================|"));
}

void CommandHandler::onCommandHelp()
{
	Serial.println(F("Command: *IDN? \r\nDescription: Returns the identity of the controller\r\n"));
	Serial.println(F("Command: VER \r\nDescription: Gets firmware version\r\n"));
	Serial.println(F("Command: CHANGELOG \r\nDescription: Gets most recent change\r\n"));
	Serial.println(F("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n"));

	Serial.println(F("Command: P \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets PGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: I \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets IGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: D \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets DGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: W \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets integral windup for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: R \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets the max rate of change for selected channel's output\r\n"));
	Serial.println(F("Command: L \r\nArg: None or Value in milliseconds\r\nDescription: Gets or sets interval for PID control loop\r\n"));

	Serial.println(F("Command: O \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in percent\r\nDescription: Gets or sets the percentage motor output for the selected channel\r\n"));
	Serial.println(F("Command: SP \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in degrees\r\nDescription: Gets or sets the set point angle for the designated motor\r\n"));
	Serial.println(F("Command: A \r\nArg: Channel (0 or 1) \r\nDescription: Gets the angle for the selected channel\r\n"));
	Serial.println(F("Command: DC \r\nArg1: Channel (0 or 1) \r\nArg2: None, or Value (CW, CCW)\r\nDescription: Gets or sets the motor direction for the selected channel\r\n"));
	Serial.println(F("Command: DV \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value (Analog, Frequency)\r\nDescription: Gets or sets the motor driver type for the selected channel\r\n"));
	Serial.println(F("Command: DAC \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in volts\r\nDescription: Gets or sets the DAC voltage for selected channel\r\n"));
	Serial.println(F("Command: F \r\nArg: None or Value in Hertz\r\nDescription: Gets or sets the frequency output\r\n"));
	Serial.println(F("Command: ADC \r\nArg: Channel (0 to 7) \r\nDescription: Reads the voltage from the selected ADC channel\r\n"));

	Serial.println(F("Command: PID \r\nArg: ON or OFF \r\nDescription: Enables or disables PID loop control\r\n"));
	Serial.println(F("Command: SAFETY \r\nArg: ON, OFF \r\nDescription: Enables or disables direct changing of motor control outputs\r\n"));
	Serial.println(F("Command: VERBOSE \r\nArg: ON or OFF \r\nDescription: Enables or disables additional debugging info\r\n"));
}

void CommandHandler::handleCommandUnknown(char* command)
{
	Serial << F("Unknown command [") << command << F("]");

	for (int i = 0; i < Utility::INT_ParameterCountMax; i++)
	{
		if (strlen(gParameters[i]))
		{
			Serial << F(" ") << i << F("{") << gParameters[i] << F("}");
		}
	}

	Serial << Utility::STR_NewLine;

	sendNack();
}

// Implementation of sending had to change due to arduino storing string literals in dynamic memory
// Used F() to force storage of literals in PROGMEM, which requires directly using Serial.println() to send
// This is because Serial.print has the proper overload to handle [const PROGMEM char *] conversion
// Source: https://www.arduino.cc/en/Reference/PROGMEM

void CommandHandler::sendNack()
{
	Serial.println(F("ERROR"));
}

void CommandHandler::sendAck()
{
	Serial.println(F("OK"));
}

void CommandHandler::sendInt(int num)
{
	Serial << num << Utility::STR_NewLine;
}

void CommandHandler::sendDouble(double num)
{
	Serial << num << Utility::STR_NewLine;
}

void CommandHandler::sendOnOffStatus(bool isOn)
{
	Serial << (isOn ? F("ON") : F("OFF")) << Utility::STR_NewLine;
}

void CommandHandler::sendOneOrZeroStatus(bool isHigh)
{
	Serial << (isHigh ? 1 : 0) << Utility::STR_NewLine;
}

void CommandHandler::sendDirectionStatus(Direction direction)
{
	Serial << (direction == Direction::Clockwise ? F("CW") : F("CCW")) << Utility::STR_NewLine;
}

void CommandHandler::sendMotorDriverStatus(MotorDriverType motorDriverType)
{
	Serial << (motorDriverType == MotorDriverType::AnalogVoltage ? F("AV") : F("F")) << Utility::STR_NewLine;
}

void CommandHandler::sendIntRangeError(int lowerLimit, int upperLimit, const char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << Utility::STR_NewLine;
	sendNack();
}

void CommandHandler::sendDoubleRangeError(double lowerLimit, double upperLimit, const char* unit)
{
	Serial << F("Value must be between ") << lowerLimit << unit << F(" and ") << upperLimit << unit << Utility::STR_NewLine;
	sendNack();
}

void CommandHandler::sendChannelError()
{
	Serial.println(F("Invalid channel"));
	sendNack();
}

void CommandHandler::sendSyntaxError()
{
	Serial.println(F("Check syntax"));
	sendNack();
}

void CommandHandler::sendReadOnlyError()
{
	Serial.println(F("Read only commands have no additional arguments"));
	sendNack();
}

void CommandHandler::sendOnOffError()
{
	Serial.println(F("Value must be ON or OFF"));
	sendNack();
}

void CommandHandler::sendOneOrZeroError()
{
	Serial.println(F("Value must be 0 or 1"));
	sendNack();
}

void CommandHandler::sendDirectionError()
{
	Serial.println(F("Value must be CLOCKWISE / CW or COUNTERCLOCKWISE / CCW"));
	sendNack();
}

void CommandHandler::sendMotorDriverError()
{
	Serial.println(F("Value must be ANALOGVOLTAGE / AV or FREQUENCY / F"));
	sendNack();
}

bool CommandHandler::isOnCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "ON") || 0 == Utility::stricmp(arg, "1"));
}

bool CommandHandler::isOffCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "OFF") || 0 == Utility::stricmp(arg, "0"));
}

bool CommandHandler::isClockwiseCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "CLOCKWISE") || 0 == Utility::stricmp(arg, "CW"));
}

bool CommandHandler::isCounterClockwiseCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "COUNTERCLOCKWISE") || 0 == Utility::stricmp(arg, "CCW"));
}

bool CommandHandler::isAnalogVoltageCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "ANALOGVOLTAGE") || 0 == Utility::stricmp(arg, "AV"));
}

bool CommandHandler::isFrequencyCommandArg(char* arg)
{
	Utility::upperCaseString(arg);
	return (0 == Utility::stricmp(arg, "FREQUENCY") || 0 == Utility::stricmp(arg, "F"));
}

bool CommandHandler::isReadCommand(char* arg)
{
	return (0 == Utility::stricmp(arg, ""));
}

bool CommandHandler::isIntWithinRange(int number, int lowerLimit, int upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool CommandHandler::isDoubleWithinRange(double number, double lowerLimit, double upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool CommandHandler::isChannelCorrect(char* channelArg)
{
	char channelStr[3];

	for (byte channel = 0; channel < Utility::INT_MaxNumChannels; channel++)
	{
		sprintf(channelStr, "%d", channel);
		if (0 == Utility::stricmp(channelArg, channelStr))
		{
			return true;
		}
	}

	return false;
}