/*
Name: commandHandlers.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
#include <Streaming.h>
#include "globals.h"
#include "util.h"
#include "commandHandlers.h"
#include "pidControl.h"

void onCommandIdentity()
{
	Serial.println(F("Arduino Helicopter Controller"));
	sendAck();
}

void onCommandEcho()
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

void onCommandChangelog()
{
	if (isReadCommand(gParameters[0]))
	{
		Serial.println(F("Fixed frequency being int instead of uint"));
		sendAck();
	}
	else
	{
		sendReadOnlyError();
	}
}

void onCommandVersion()
{
	if (isReadCommand(gParameters[0]))
	{
		sendDouble(FIRMWARE_VERSION);
		sendAck();
	}
	else
	{
		sendReadOnlyError();
	}
}

void onCommandPidControl()
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(isPidEnabled);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		if (!isPidEnabled)
		{
			enablePid();
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
		disablePid();
		sendAck();
	}
	else
	{
		sendOnOffError();
	}
}

void onCommandVerbose()
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(isVerboseMode);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		isVerboseMode = true;
		sendAck();
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		isVerboseMode = false;
		sendAck();
	}
	else
	{
		sendOnOffError();
	}
}

void onCommandSafety()
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(isSafetyOn);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		isSafetyOn = true;
		sendAck();
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		isSafetyOn = false;
		sendAck();
	}
	else
	{
		sendOnOffError();
	}
}

void onCommandOutput()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		int output = convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(currentOutputs[channel]);
			sendAck();
		}
		else if (!isPidEnabled)
		{
			if (isIntWithinRange(output, PID_OUTPUT_MIN, PID_OUTPUT_MAX))
			{
				const Direction direction = directions[channel];

				applyMotorOutputs(channel, direction, output);
				sendAck();
			}
			else
			{
				sendIntRangeError(PID_OUTPUT_MIN, PID_OUTPUT_MAX, PERCENTAGE_UNIT);
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
void onCommandDirection()
{
	if (isChannelCorrect(gParameters[0]))
	{
		uint8_t channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDirectionStatus(directions[channel]);
			sendAck();
		}
		else if (!isPidEnabled)
		{
			int output = currentOutputs[channel];
			int outputRateLimit = outputRateLimits[channel];
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

			if (direction != directions[channel] && output > outputRateLimit / 2)
			{
				Serial << F("Cannot make a change to output by more than ") << outputRateLimit << F("%") << NEWLINE;
				sendNack();
			}
			else
			{
				applyMotorOutputs(channel, direction, output);
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

void onCommandMotorDriver()
{
	if (isChannelCorrect(gParameters[0]))
	{
		uint8_t channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendMotorDriverStatus(motorDriverTypes[channel]);
			sendAck();
		}
		else
		{
			if (!isPidEnabled)
			{
				if (isAnalogVoltageCommandArg(gParameters[1]))
				{
					motorDriverTypes[channel] = AnalogVoltage;
					sendAck();
				}
				else if (isFrequencyCommandArg(gParameters[1]))
				{
					if (channel == TILT_CHANNEL)
					{
						motorDriverTypes[channel] = Frequency;
						sendAck();
					}
					else
					{
						Serial.println(F("Yaw (Channel 1) can only be set to Analog Voltage"));
						sendNack();
					}
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
	}
	else
	{
		sendChannelError();
	}
}

void onCommandProportionalGain()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isProfileCorrect(gParameters[1]))
		{
			int profile = convertToInt(gParameters[1]);
			double pGain = atof(gParameters[2]);

			if (isReadCommand(gParameters[2]))
			{
				sendDouble(pGains[channel][profile]);
				sendAck();
			}
			else if (isDoubleWithinRange(pGain, P_GAIN_MIN, P_GAIN_MAX))
			{
				pGains[channel][profile] = pGain;
				sendAck();
			}
			else
			{
				sendDoubleRangeError(P_GAIN_MIN, P_GAIN_MAX, NO_UNIT);
			}
		}
		else
		{
			sendProfileError();
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandIntegralGain()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isProfileCorrect(gParameters[1]))
		{
			int profile = convertToInt(gParameters[1]);
			double iGain = atof(gParameters[2]);

			if (isReadCommand(gParameters[2]))
			{
				sendDouble(iGains[channel][profile]);
				sendAck();
			}
			else if (isDoubleWithinRange(iGain, I_GAIN_MIN, P_GAIN_MAX))
			{
				iGains[channel][profile] = iGain;
				sendAck();
			}
			else
			{
				sendDoubleRangeError(I_GAIN_MIN, I_GAIN_MAX, NO_UNIT);
			}
		}
		else
		{
			sendProfileError();
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandDerivativeGain()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isProfileCorrect(gParameters[1]))
		{
			int profile = convertToInt(gParameters[1]);
			double dGain = atof(gParameters[2]);

			if (isReadCommand(gParameters[2]))
			{
				sendDouble(dGains[channel][profile]);
				sendAck();
			}
			else if (isDoubleWithinRange(dGain, D_GAIN_MIN, D_GAIN_MAX))
			{
				dGains[channel][profile] = dGain;
				sendAck();
			}
			else
			{
				sendDoubleRangeError(D_GAIN_MIN, D_GAIN_MAX, NO_UNIT);
			}
		}
		else
		{
			sendProfileError();
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandSetPoint()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		double setPoint = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(setPoints[channel]);
			sendAck();
		}
		else if (isDoubleWithinRange(setPoint, minSetPoint[channel], maxSetPoint[channel]))
		{
			setPoints[channel] = setPoint;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(minSetPoint[channel], maxSetPoint[channel], DEGREES_UNIT);
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandLoopInterval()
{
	int loopInterval = convertToInt(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendInt(pidLoopInterval);
		sendAck();
	}
	else if (isIntWithinRange(loopInterval, PID_INTERVAL_MS_MIN, PID_INTERVAL_MS_MAX))
	{
		if (!isPidEnabled)
		{
			pidLoopInterval = loopInterval;
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
		sendIntRangeError(PID_INTERVAL_MS_MIN, PID_INTERVAL_MS_MAX, MILLISECONDS_UNIT);
	}
}

void onCommandIntegralWindup()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		double windup = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(iWindupThresholds[channel]);
			sendAck();
		}
		else if (isDoubleWithinRange(windup, I_WINDUP_THRESH_MIN, I_WINDUP_THRESH_MAX))
		{
			iWindupThresholds[channel] = windup;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(I_WINDUP_THRESH_MIN, I_WINDUP_THRESH_MAX, DEGREES_UNIT);
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandRateLimit()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		int rateLimit = convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(outputRateLimits[channel]);
			sendAck();
		}
		else if (isIntWithinRange(rateLimit, OUTPUT_RATE_LIMIT_MIN, OUTPUT_RATE_LIMIT_MAX))
		{
			outputRateLimits[channel] = rateLimit;
			sendAck();
		}
		else
		{
			sendIntRangeError(OUTPUT_RATE_LIMIT_MIN, OUTPUT_RATE_LIMIT_MAX, DEGREES_UNIT);
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandAngle()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(currentAngles[channel]);
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

void onCommandZeroEncoderAngle()
{
	if (isReadCommand(gParameters[1]))
	{
		zeroEncoderAngle();
		sendAck();
	}
	else
	{
		sendReadOnlyError();
	}
}

void onCommandAdc()
{
	int adcChannel = convertToInt(gParameters[0]);

	if (isIntWithinRange(adcChannel, ADC_CHANNEL_MIN, ADC_CHANNEL_MAX))
	{
		double voltage = getSampledAdcVoltage(adcChannel);
		sendDouble(voltage);
		sendAck();
	}
	else
	{
		sendIntRangeError(ADC_CHANNEL_MIN, ADC_CHANNEL_MAX, NO_UNIT);
	}
}

void onCommandDacVoltage()
{
	if (isChannelCorrect(gParameters[0]))
	{
		uint8_t channel = convertToInt(gParameters[0]);
		double voltage = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(currentVoltages[channel]);
			sendAck();
		}
		else
		{
			if (!isSafetyOn)
			{
				if (isDoubleWithinRange(voltage, MOTOR_MIN_VOLTAGE, MOTOR_MAX_VOLTAGE))
				{
					setDacVoltage(channel, voltage);
					sendAck();
				}
				else
				{
					sendDoubleRangeError(MOTOR_MIN_VOLTAGE, MOTOR_MAX_VOLTAGE, VOLTAGE_UNIT);
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
void onCommandFrequencyOutput()
{
	if (isChannelCorrect(gParameters[0]))
	{
		uint8_t channel = convertToInt(gParameters[0]);
		uint16_t frequency = convertToUint(gParameters[1]);

		if (channel == TILT_CHANNEL)
		{
			if (isReadCommand(gParameters[1]))
			{
				sendInt(currentFrequency);
				sendAck();
			}
			else
			{
				if (!isSafetyOn)
				{
					if (isIntWithinRange(frequency, MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY))
					{
						setFrequency(frequency);
						sendAck();
					}
					else
					{
						sendIntRangeError(MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY, HERTZ_UNIT);
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

void onCommandTest()
{
}

void onCommandHelp()
{
	Serial.println(F("Command: *IDN? \r\nDescription: Returns the identity of the controller\r\n"));
	Serial.println(F("Command: VER \r\nDescription: Gets firmware version\r\n"));
	Serial.println(F("Command: CHANGELOG \r\nDescription: Gets most recent change\r\n"));
	Serial.println(F("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n"));

	Serial.println(F("Command: P \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets PGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: I \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets IGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: D \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets DGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: IW \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets integral windup for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: RL \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets the rate of change limit for selected channel's output\r\n"));
	Serial.println(F("Command: LI \r\nArg: None or Value in milliseconds\r\nDescription: Gets or sets the PID control loop interval\r\n"));

	Serial.println(F("Command: O \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in percent\r\nDescription: Gets or sets the percentage motor output for the selected channel\r\n"));
	Serial.println(F("Command: SP \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in degrees\r\nDescription: Gets or sets the set point angle for the designated motor\r\n"));
	Serial.println(F("Command: A \r\nArg: Channel (0 or 1) \r\nDescription: Gets the angle for the selected channel\r\n"));
	Serial.println(F("Command: Z \r\nDescription: Zeroes the encoder angle on channel 0 (yaw)\r\n"));
	Serial.println(F("Command: DC \r\nArg1: Channel (0 or 1) \r\nArg2: None, or Value (CW, CCW)\r\nDescription: Gets or sets the motor direction for the selected channel\r\n"));
	Serial.println(F("Command: DV \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value (Analog, Frequency)\r\nDescription: Gets or sets the motor driver type for the selected channel\r\n"));
	Serial.println(F("Command: DAC \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in volts\r\nDescription: Gets or sets the DAC voltage for selected channel\r\n"));
	Serial.println(F("Command: F \r\nArg: None or Value in Hertz\r\nDescription: Gets or sets the frequency output\r\n"));
	Serial.println(F("Command: ADC \r\nArg: Channel (0 to 7) \r\nDescription: Reads the voltage from the selected ADC channel\r\n"));

	Serial.println(F("Command: PID \r\nArg: ON or OFF \r\nDescription: Enables or disables PID loop control\r\n"));
	Serial.println(F("Command: SAFETY \r\nArg: ON, OFF \r\nDescription: Enables or disables direct changing of motor control outputs\r\n"));
	Serial.println(F("Command: VERBOSE \r\nArg: ON or OFF \r\nDescription: Enables or disables additional debugging info\r\n"));
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

	Serial.println(tmpstr);
	sendNack();
}