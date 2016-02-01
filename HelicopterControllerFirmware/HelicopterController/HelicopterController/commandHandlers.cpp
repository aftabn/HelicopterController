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
		enablePid();
		sendAck();
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
				currentOutputs[channel] = output;
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
			if (isClockwiseCommandArg(gParameters[1]))
			{
				directions[channel] = Clockwise;
				sendAck();
			}
			else if (isCounterClockwiseCommandArg(gParameters[1]))
			{
				directions[channel] = CounterClockwise;
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
		pidLoopInterval = loopInterval;
		sendAck();
	}
	else
	{
		sendIntRangeError(PID_INTERVAL_MS_MIN, PID_INTERVAL_MS_MAX, MILLISECONDS_UNIT);
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
				Serial.println(F("Reading of voltage is not implemented"));
				sendNack();
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
	int frequency = convertToInt(gParameters[0]);

	if (!isSafetyOn)
	{
		if (isReadCommand(gParameters[0]))
		{
			Serial.println(F("Reading of frequency is not implemented"));
			sendNack();
		}
		else if (isIntWithinRange(frequency, MOTOR_MIN_FREQUENCY, MOTOR_MAX_FREQUENCY))
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

void onCommandTest()
{
}

// TODO: Update this
void onCommandState()
{
	char tmpstr[50];
	Serial.println(F("|===============================|"));
	Serial.println(F("| Channel\t0\t1\t|"));
	Serial.println(F("|-------------------------------|"));
	sprintf(tmpstr, "| Output\t%d\t%d\t|", currentOutputs[0], currentOutputs[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| Angles\t%.2f\t%.2f\t|", currentAngles[0], currentAngles[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| P-Gain\t%.2f\t%.2f\t|", pGains[0], pGains[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| I-Gain\t%.2f\t%.2f\t|", iGains[0], iGains[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| D-Gain\t%.2f\t%.2f\t|", dGains[0], dGains[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| Set Points\t%.2f\t%.2f\t|", setPoints[0], setPoints[1]);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| PID Interval\t%d ms\t\t|", pidLoopInterval);
	Serial.println(tmpstr);
	sprintf(tmpstr, "| PID Control\t%s\t\t|", isPidEnabled ? "On" : "Off");
	Serial.println(tmpstr);
	sprintf(tmpstr, "| Safety\t%s\t\t|", isSafetyOn ? "On" : "Off");
	Serial.println(tmpstr);
	sprintf(tmpstr, "| Debug\t\t%s\t\t|", isDebugMode ? "On" : "Off");
	Serial.println(tmpstr);
	Serial.println("|===============================|");
}

void onCommandHelp()
{
	Serial.println(F("Command: *IDN? \r\nDescription: Returns xxx-xxxxxA-B (Kardium Part Number)\r\n"));
	Serial.println(F("Command: SYS \r\nArg: VER \r\nDescription: Gets information about jig or the controller\r\n"));
	Serial.println(F("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n"));
	Serial.println(F("Command: P \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets PGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: I \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets IGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: D \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value\r\nDescription: Gets or sets DGain for selected channel's PID control loop\r\n"));
	Serial.println(F("Command: INTERVAL \r\nArg: None or Value in milliseconds\r\nDescription: Gets or sets interval for PID control loop\r\n"));

	Serial.println(F("Command: OUTPUT \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in percent\r\nDescription: Gets or sets the percentage motor output for the selected channel\r\n"));
	Serial.println(F("Command: SP \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in degrees\r\nDescription: Gets or sets the set point angle for the designated motor\r\n"));
	Serial.println(F("Command: DIRECTION \r\nArg1: Channel (0 or 1) \r\nArg2: None, or Value (CW, CCW)\r\nDescription: Gets or sets the motor direction for the selected channel\r\n"));
	Serial.println(F("Command: DRIVER \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value (Analog, Frequency)\r\nDescription: Gets or sets the motor driver type for the selected channel\r\n"));
	Serial.println(F("Command: VOLTAGE \r\nArg1: Channel (0 or 1) \r\nArg2: None or Value in volts\r\nDescription: Gets or sets the DAC voltage for selected channel\r\n"));
	Serial.println(F("Command: FREQUENCY \r\nArg: None or Value in Hertz\r\nDescription: Gets or sets the frequency output\r\n"));

	Serial.println(F("Command: PID \r\nArg: ON or OFF \r\nDescription: Enables or disables PID loop control\r\n"));
	Serial.println(F("Command: SAFETY \r\nArg: ON, OFF \r\nDescription: Enables or disables direct changing of motor control outputs\r\n"));
	Serial.println(F("Command: STATUS \r\nArg: None \r\nDescription: Returns debugging info regarding controller variables\r\n"));
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