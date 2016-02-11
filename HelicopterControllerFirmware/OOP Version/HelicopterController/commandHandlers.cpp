/*
Name: commandHandlers.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <avr\pgmspace.h>
#include "globals.h"
#include "util.h"
#include "commandHandlers.h"
#include "pidControl.h"

void onCommandIdentity()
{
	Serial.println(CONTROLLER_NAME);
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

void onCommandSystem()
{
	if (0 == stricmp(gParameters[0], "VER"))
	{
		sendDouble(FIRMWARE_VERSION, DEFAULT_NUM_DECIMALS);
		sendAck();
	}
	else
	{
		sendSyntaxError();
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
		if (isPidEnabled)
		{
			disablePid();
			sendAck();
		}
		else
		{
			Serial.println(F("PID control is already off."));
			sendNack();
		}
	}
	else
	{
		sendOnOffError();
	}
}

void onCommandDebug()
{
	if (isReadCommand(gParameters[0]))
	{
		sendOnOffStatus(isDebugMode);
		sendAck();
	}
	else if (isOnCommandArg(gParameters[0]))
	{
		isDebugMode = true;
		sendAck();
	}
	else if (isOffCommandArg(gParameters[0]))
	{
		isDebugMode = false;
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

void onCommandDirection()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDirectionStatus(directions[channel]);
			sendAck();
		}
		else if (!isPidEnabled)
		{
			const int output = currentOutputs[channel];

			if (isClockwiseCommandArg(gParameters[1]))
			{
				applyMotorOutputs(channel, Clockwise, output);
				sendAck();
			}
			else if (isCounterClockwiseCommandArg(gParameters[1]))
			{
				applyMotorOutputs(channel, CounterClockwise, output);
				sendAck();
			}
			else
			{
				sendDirectionError();
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
		int channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendMotorDriverStatus(motorDriverTypes[channel]);
			sendAck();
		}
		else if (!isPidEnabled)
		{
			if (isAnalogVoltageCommandArg(gParameters[1]))
			{
				motorDriverTypes[channel] = AnalogVoltage;
				sendAck();
			}
			else if (isFrequencyCommandArg(gParameters[1]))
			{
				motorDriverTypes[channel] = Frequency;
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

void onCommandProportionalGain()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		double pGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(pGains[channel], DEFAULT_NUM_DECIMALS);
			sendAck();
		}
		else if (isDoubleWithinRange(pGain, P_GAIN_MIN, P_GAIN_MAX))
		{
			pGains[channel] = pGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(P_GAIN_MIN, P_GAIN_MAX, NO_UNIT);
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
		double iGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(iGains[channel], DEFAULT_NUM_DECIMALS);
			sendAck();
		}
		else if (isDoubleWithinRange(iGain, I_GAIN_MIN, P_GAIN_MAX))
		{
			iGains[channel] = iGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(I_GAIN_MIN, I_GAIN_MAX, NO_UNIT);
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
		double dGain = atof(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(dGains[channel], DEFAULT_NUM_DECIMALS);
			sendAck();
		}
		else if (isDoubleWithinRange(dGain, D_GAIN_MIN, D_GAIN_MAX))
		{
			dGains[channel] = dGain;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(D_GAIN_MIN, D_GAIN_MAX, NO_UNIT);
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
			sendDouble(setPoints[channel], DEFAULT_NUM_DECIMALS);
			sendAck();
		}
		else if (isDoubleWithinRange(setPoint, SET_POINT_MIN, SET_POINT_MAX))
		{
			setPoints[channel] = setPoint;
			sendAck();
		}
		else
		{
			sendDoubleRangeError(SET_POINT_MIN, SET_POINT_MAX, DEGREES_UNIT);
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

void onCommandAngle()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);

		if (isReadCommand(gParameters[1]))
		{
			sendDouble(currentAngles[channel], DEFAULT_NUM_DECIMALS);
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

void onCommandAdc()
{
	int adcChannel = convertToInt(gParameters[0]);

	if (isIntWithinRange(adcChannel, ADC_CHANNEL_MIN, ADC_CHANNEL_MAX))
	{
		double voltage = getAdcVoltage(adcChannel);
		sendDouble(voltage, THREE_DECIMALS);
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
		if (!isSafetyOn)
		{
			int channel = convertToInt(gParameters[0]);
			double voltage = atof(gParameters[1]);

			if (isReadCommand(gParameters[1]))
			{
				sendDouble(currentVoltages[channel], DEFAULT_NUM_DECIMALS);
				sendAck();
			}
			else if (isDoubleWithinRange(voltage, MOTOR_MIN_VOLTAGE, MOTOR_MAX_VOLTAGE))
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
			Serial.println(F("Cannot change DAC voltage while safety is on."));
			sendNack();
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandFrequencyOutput()
{
	if (isChannelCorrect(gParameters[0]))
	{
		if (!isSafetyOn)
		{
			int channel = convertToInt(gParameters[0]);
			int frequency = convertToInt(gParameters[1]);

			if (isReadCommand(gParameters[1]))
			{
				sendInt(currentFrequency);
				sendAck();
			}
			else if (isIntWithinRange(frequency, MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY))
			{
				setFrequency(channel, frequency);
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
	else
	{
		sendChannelError();
	}
}

void onCommandTest()
{
}

void onCommandState()
{
	//char tmpstr[40];
	//char num1[8];
	//char num2[8];
	//
	//Serial.println(F("|===============================|"));
	//Serial.println(F("| Channel\t0\t1\t|"));
	//Serial.println(F("|-------------------------------|"));
	//
	//sprintf(tmpstr, "| Output\t%d\t%d\t|", currentOutputs[0], currentOutputs[1]);
	//Serial.println(tmpstr);
	//
	//switch (directions[0])
	//{
	//case Clockwise:
	//	strcpy(num1, "CW");
	//	break;
	//case CounterClockwise:
	//	strcpy(num1, "CCW");
	//	break;
	//default:
	//	strcpy(num1, "N/A");
	//	break;
	//}
	//
	//switch (directions[1])
	//{
	//case Clockwise:
	//	strcpy(num2, "CW");
	//	break;
	//case CounterClockwise:
	//	strcpy(num2, "CCW");
	//	break;
	//default:
	//	strcpy(num2, "N/A");
	//	break;
	//}
	//
	//sprintf(tmpstr, "| Directions\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(currentAngles[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(currentAngles[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| Angles\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(pGains[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(pGains[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| P-Gain\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(iGains[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(iGains[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| I-Gain\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(dGains[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(dGains[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| D-Gain\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(setPoints[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(setPoints[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| Set Points\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//switch (motorDriverTypes[0])
	//{
	//case AnalogVoltage:
	//	strcpy(num1, "A.V.");
	//	break;
	//case Frequency:
	//	strcpy(num1, "Freq.");
	//	break;
	//default:
	//	strcpy(num1, "N/A");
	//	break;
	//}
	//
	//switch (motorDriverTypes[1])
	//{
	//case AnalogVoltage:
	//	strcpy(num2, "A.V.");
	//	break;
	//case Frequency:
	//	strcpy(num2, "Freq.");
	//	break;
	//default:
	//	strcpy(num2, "N/A");
	//	break;
	//}
	//
	//sprintf(tmpstr, "| Drivers\t%s\t%s\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//dtostrf(currentVoltages[0], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num1);
	//dtostrf(currentVoltages[1], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, num2);
	//sprintf(tmpstr, "| DAC\t\t%sV\t%sV\t|", num1, num2);
	//Serial.println(tmpstr);
	//
	//sprintf(tmpstr, "| Frequency\t%d Hz\t\t|", currentFrequency);
	//Serial.println(tmpstr);
	//
	//sprintf(tmpstr, "| PID Interval\t%d ms\t\t|", pidLoopInterval);
	//Serial.println(tmpstr);
	//sprintf(tmpstr, "| PID Control\t%s\t\t|", isPidEnabled ? "On" : "Off");
	//Serial.println(tmpstr);
	//sprintf(tmpstr, "| Safety\t%s\t\t|", isSafetyOn ? "On" : "Off");
	//Serial.println(tmpstr);
	//sprintf(tmpstr, "| Debug\t\t%s\t\t|", isDebugMode ? "On" : "Off");
	//Serial.println(tmpstr);
	//Serial.println("|===============================|");
}

void onCommandHelp()
{
	Serial.println(F("Command: *IDN? \r\nDescription: Returns the identity of the controller\r\n"));
	Serial.println(F("Command: SYS \r\nArg: VER \r\nDescription: Gets information about jig or the controller\r\n"));
	Serial.println(F("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n"));
	Serial.println(F("Command: P \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets PGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: I \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets IGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: D \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets DGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: LOOP \r\nArg: None or Value in milliseconds\r\nDescription: Gets or sets interval for PID control loop\r\n"));

	Serial.println(F("Command: OUT \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in percent\r\nDescription: Gets or sets the percentage motor output for the selected channel\r\n"));
	Serial.println(F("Command: SP \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in degrees\r\nDescription: Gets or sets the set point angle for the designated motor\r\n"));
	Serial.println(F("Command: DIR \r\nArg1: Channel (0 or 1) \r\nArg2: None, or Value (CW, CCW)\r\nDescription: Gets or sets the motor direction for the selected channel\r\n"));
	Serial.println(F("Command: DRIVER \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value (Analog, Frequency)\r\nDescription: Gets or sets the motor driver type for the selected channel\r\n"));
	Serial.println(F("Command: DAC \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in volts\r\nDescription: Gets or sets the DAC voltage for selected channel\r\n"));
	Serial.println(F("Command: FREQ \r\nArg: None or Value in Hertz\r\nDescription: Gets or sets the frequency output\r\n"));
	Serial.println(F("Command: ADC \r\nArg: Channel (0 to 7) \r\nDescription: Reads the voltage from the selected ADC channel\r\n"));

	Serial.println(F("Command: PID \r\nArg: ON or OFF \r\nDescription: Enables or disables PID loop control\r\n"));
	Serial.println(F("Command: SAFETY \r\nArg: ON, OFF \r\nDescription: Enables or disables direct changing of motor control outputs\r\n"));
	Serial.println(F("Command: STATE \r\nArg: None \r\nDescription: Returns debugging info regarding controller variables\r\n"));
	Serial.println(F("Command: DEBUG \r\nArg: ON or OFF \r\nDescription: Enables or disables additional debugging info\r\n"));
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