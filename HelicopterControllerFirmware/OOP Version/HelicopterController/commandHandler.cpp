/*
Name: commandHandler.cpp
Created: 1/10/2016
Author:	Aftab
*/

#include "commandHandler.h"

char CommandHandler::gParameters[Utility::INT_ParameterCountMax][Utility::INT_ParameterLengthMax + 1];

void CommandHandler::onCommandIdentity()
{
	Serial.println(F("Arduino Helicopter Controller"));
	Utility::sendAck();
}

void CommandHandler::onCommandEcho()
{
	if (CommandHandler::gParameters[0] != NULL)
	{
		Serial.println(CommandHandler::gParameters[0]);
		Utility::sendAck();
	}
	else
	{
		Utility::sendNack();
	}
}

void CommandHandler::onCommandChangelog()
{
	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
	{
		Serial.println(F("Fixed outputs not updating from disabling PID"));
		Utility::sendAck();
	}
	else
	{
		Utility::sendReadOnlyError();
	}
}

void CommandHandler::onCommandVersion()
{
	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
	{
		Utility::sendDouble(1.07);
		Utility::sendAck();
	}
	else
	{
		Utility::sendReadOnlyError();
	}
}
//
//void onCommandPidControl()
//{
//	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
//	{
//		Utility::sendOnOffStatus(isPidEnabled);
//		Utility::sendAck();
//	}
//	else if (isOnCommandArg(CommandHandler::gParameters[0]))
//	{
//		if (!isPidEnabled)
//		{
//			enablePid();
//			Utility::sendAck();
//		}
//		else
//		{
//			Serial.println(F("PID control is already on."));
//			Utility::sendNack();
//		}
//	}
//	else if (isOffCommandArg(CommandHandler::gParameters[0]))
//	{
//		disablePid();
//		Utility::sendAck();
//	}
//	else
//	{
//		sendOnOffError();
//	}
//}
//
//void onCommandVerbose()
//{
//	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
//	{
//		sendOnOffStatus(isVerboseMode);
//		Utility::sendAck();
//	}
//	else if (isOnCommandArg(CommandHandler::gParameters[0]))
//	{
//		isVerboseMode = true;
//		Utility::sendAck();
//	}
//	else if (isOffCommandArg(CommandHandler::gParameters[0]))
//	{
//		isVerboseMode = false;
//		Utility::sendAck();
//	}
//	else
//	{
//		sendOnOffError();
//	}
//}
//
//void onCommandSafety()
//{
//	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
//	{
//		sendOnOffStatus(isSafetyOn);
//		Utility::sendAck();
//	}
//	else if (isOnCommandArg(CommandHandler::gParameters[0]))
//	{
//		isSafetyOn = true;
//		Utility::sendAck();
//	}
//	else if (isOffCommandArg(CommandHandler::gParameters[0]))
//	{
//		isSafetyOn = false;
//		Utility::sendAck();
//	}
//	else
//	{
//		sendOnOffError();
//	}
//}
//
//void onCommandOutput()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		int output = convertToInt(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendInt(currentOutputs[channel]);
//			Utility::sendAck();
//		}
//		else if (!isPidEnabled)
//		{
//			if (isIntWithinRange(output, PID_OUTPUT_MIN, PID_OUTPUT_MAX))
//			{
//				const Direction direction = directions[channel];
//
//				applyMotorOutputs(channel, direction, output);
//				Utility::sendAck();
//			}
//			else
//			{
//				sendIntRangeError(PID_OUTPUT_MIN, PID_OUTPUT_MAX, PERCENTAGE_UNIT);
//			}
//		}
//		else
//		{
//			Serial.println(F("Cannot change percentage output while PID control is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandDirection()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDirectionStatus(directions[channel]);
//			Utility::sendAck();
//		}
//		else if (!isPidEnabled)
//		{
//			const int output = currentOutputs[channel];
//
//			if (isClockwiseCommandArg(CommandHandler::gParameters[1]))
//			{
//				applyMotorOutputs(channel, Clockwise, output);
//				Utility::sendAck();
//			}
//			else if (isCounterClockwiseCommandArg(CommandHandler::gParameters[1]))
//			{
//				applyMotorOutputs(channel, CounterClockwise, output);
//				Utility::sendAck();
//			}
//			else
//			{
//				sendDirectionError();
//			}
//		}
//		else
//		{
//			Serial.println(F("Cannot change direction while PID control is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandMotorDriver()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendMotorDriverStatus(motorDriverTypes[channel]);
//			Utility::sendAck();
//		}
//		else if (!isPidEnabled)
//		{
//			if (isAnalogVoltageCommandArg(CommandHandler::gParameters[1]))
//			{
//				motorDriverTypes[channel] = AnalogVoltage;
//				Utility::sendAck();
//			}
//			else if (isFrequencyCommandArg(CommandHandler::gParameters[1]))
//			{
//				motorDriverTypes[channel] = Frequency;
//				Utility::sendAck();
//			}
//			else
//			{
//				sendMotorDriverError();
//			}
//		}
//		else
//		{
//			Serial.println(F("Cannot change driver type while PID control is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandProportionalGain()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		double pGain = atof(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(pGains[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else if (isDoubleWithinRange(pGain, P_GAIN_MIN, P_GAIN_MAX))
//		{
//			pGains[channel] = pGain;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendDoubleRangeError(P_GAIN_MIN, P_GAIN_MAX, NO_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandIntegralGain()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		double iGain = atof(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(iGains[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else if (isDoubleWithinRange(iGain, I_GAIN_MIN, P_GAIN_MAX))
//		{
//			iGains[channel] = iGain;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendDoubleRangeError(I_GAIN_MIN, I_GAIN_MAX, NO_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandDerivativeGain()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		double dGain = atof(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(dGains[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else if (isDoubleWithinRange(dGain, D_GAIN_MIN, D_GAIN_MAX))
//		{
//			dGains[channel] = dGain;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendDoubleRangeError(D_GAIN_MIN, D_GAIN_MAX, NO_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandSetPoint()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		double setPoint = atof(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(setPoints[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else if (isDoubleWithinRange(setPoint, SET_POINT_MIN, SET_POINT_MAX))
//		{
//			setPoints[channel] = setPoint;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendDoubleRangeError(SET_POINT_MIN, SET_POINT_MAX, DEGREES_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandLoopInterval()
//{
//	int loopInterval = convertToInt(CommandHandler::gParameters[0]);
//
//	if (Utility::isReadCommand(CommandHandler::gParameters[0]))
//	{
//		sendInt(pidLoopInterval);
//		Utility::sendAck();
//	}
//	else if (isIntWithinRange(loopInterval, PID_INTERVAL_MS_MIN, PID_INTERVAL_MS_MAX))
//	{
//		if (!isPidEnabled)
//		{
//			pidLoopInterval = loopInterval;
//			Utility::sendAck();
//		}
//		else
//		{
//			Serial.println(F("Cannot change loop interval while PID control is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendIntRangeError(PID_INTERVAL_MS_MIN, PID_INTERVAL_MS_MAX, MILLISECONDS_UNIT);
//	}
//}
//
//void onCommandIntegralWindup()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		double windup = atof(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(iWindupThresholds[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else if (isDoubleWithinRange(windup, I_WINDUP_THRESH_MIN, I_WINDUP_THRESH_MAX))
//		{
//			iWindupThresholds[channel] = windup;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendDoubleRangeError(I_WINDUP_THRESH_MIN, I_WINDUP_THRESH_MAX, DEGREES_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandRateLimit()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//		int rateLimit = convertToInt(CommandHandler::gParameters[1]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendInt(outputRateLimits[channel]);
//			Utility::sendAck();
//		}
//		else if (isIntWithinRange(rateLimit, OUTPUT_RATE_LIMIT_MIN, OUTPUT_RATE_LIMIT_MAX))
//		{
//			outputRateLimits[channel] = rateLimit;
//			Utility::sendAck();
//		}
//		else
//		{
//			sendIntRangeError(OUTPUT_RATE_LIMIT_MIN, OUTPUT_RATE_LIMIT_MAX, DEGREES_UNIT);
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandAngle()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		int channel = convertToInt(CommandHandler::gParameters[0]);
//
//		if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//		{
//			sendDouble(currentAngles[channel], DEFAULT_NUM_DECIMALS);
//			Utility::sendAck();
//		}
//		else
//		{
//			Utility::sendReadOnlyError();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandAdc()
//{
//	int adcChannel = convertToInt(CommandHandler::gParameters[0]);
//
//	if (isIntWithinRange(adcChannel, ADC_CHANNEL_MIN, ADC_CHANNEL_MAX))
//	{
//		double voltage = getSampledAdcVoltage(adcChannel);
//		sendDouble(voltage, THREE_DECIMALS);
//		Utility::sendAck();
//	}
//	else
//	{
//		sendIntRangeError(ADC_CHANNEL_MIN, ADC_CHANNEL_MAX, NO_UNIT);
//	}
//}
//
//void onCommandDacVoltage()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		if (!isSafetyOn)
//		{
//			int channel = convertToInt(CommandHandler::gParameters[0]);
//			double voltage = atof(CommandHandler::gParameters[1]);
//
//			if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//			{
//				sendDouble(currentVoltages[channel], DEFAULT_NUM_DECIMALS);
//				Utility::sendAck();
//			}
//			else if (isDoubleWithinRange(voltage, MOTOR_MIN_VOLTAGE, MOTOR_MAX_VOLTAGE))
//			{
//				setDacVoltage(channel, voltage);
//				Utility::sendAck();
//			}
//			else
//			{
//				sendDoubleRangeError(MOTOR_MIN_VOLTAGE, MOTOR_MAX_VOLTAGE, VOLTAGE_UNIT);
//			}
//		}
//		else
//		{
//			Serial.println(F("Cannot change DAC voltage while safety is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
//void onCommandFrequencyOutput()
//{
//	if (isChannelCorrect(CommandHandler::gParameters[0]))
//	{
//		if (!isSafetyOn)
//		{
//			int channel = convertToInt(CommandHandler::gParameters[0]);
//			int frequency = convertToInt(CommandHandler::gParameters[1]);
//
//			if (Utility::isReadCommand(CommandHandler::gParameters[1]))
//			{
//				sendInt(currentFrequency);
//				Utility::sendAck();
//			}
//			else if (isIntWithinRange(frequency, MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY))
//			{
//				setFrequency(channel, frequency);
//				Utility::sendAck();
//			}
//			else
//			{
//				sendIntRangeError(MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY, HERTZ_UNIT);
//			}
//		}
//		else
//		{
//			Serial.println(F("Cannot change frequency output while safety is on."));
//			Utility::sendNack();
//		}
//	}
//	else
//	{
//		sendChannelError();
//	}
//}
//
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
	char tmpstr[128];
	char newstr[40];
	sprintf(tmpstr, "Unknown command [%s]", command);

	for (int i = 0; i < Utility::INT_ParameterCountMax; i++)
	{
		if (strlen(CommandHandler::gParameters[i]))
		{
			sprintf(newstr, "  %d{%s}", i, CommandHandler::gParameters[i]);
			strcat(tmpstr, newstr);
		}
	}

	Serial.println(tmpstr);
	Utility::sendNack();
}