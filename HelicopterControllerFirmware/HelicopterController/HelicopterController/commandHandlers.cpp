//
//
//

#include "globals.h"
#include "util.h"
#include "commandHandlers.h"
#include "pidControl.h"

void onCommandIdentity()
{
	send(CONTROLLER_NAME);
	sendAck();
}

void onCommandEcho()
{
	if (gParameters[0] != NULL)
	{
		send(gParameters[0]);
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

void onCommandLoopInterval()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		int loopInterval = convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(loopIntervals[channel]);
			sendAck();
		}
		else if (isIntWithinRange(loopInterval, PID_INTERVAL_SEC_MIN, PID_INTERVAL_SEC_MAX))
		{
			loopIntervals[channel] = loopInterval;
			sendAck();
		}
		else
		{
			sendIntRangeError(PID_INTERVAL_SEC_MIN, PID_INTERVAL_SEC_MAX, MILLISECONDS_UNIT);
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

void onCommandPidLoopControl()
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

void onCommandMaxDuty()
{
	if (isReadCommand(gParameters[0]))
	{
		sendInt(gMaximumDutyCycle);
		sendAck();
	}
	else
	{
		int newMaximumDutyCycle = convertToInt(gParameters[0]);

		if (isIntWithinRange(newMaximumDutyCycle, PWM_DUTY_MIN, PWM_DUTY_MAX))
		{
			gMaximumDutyCycle = newMaximumDutyCycle;
			sendAck();
		}
		else
		{
			sendIntRangeError(PWM_DUTY_MIN, PWM_DUTY_MAX, NO_UNIT);
		}
	}
}

void onCommandDuty()
{
	if (isChannelCorrect(gParameters[0]))
	{
		int channel = convertToInt(gParameters[0]);
		int duty = convertToInt(gParameters[1]);

		if (isReadCommand(gParameters[1]))
		{
			sendInt(currentDuties[channel]);
			sendAck();
		}
		else if (!isPidEnabled)
		{
			if (isIntWithinRange(duty, PWM_DUTY_MIN, PWM_DUTY_MAX))
			{
				currentDuties[channel] = duty;
				sendAck();
			}
			else
			{
				sendIntRangeError(PWM_DUTY_MIN, PWM_DUTY_MAX, NO_UNIT);
			}
		}
		else
		{
			sendError("Cannot change duty value while PID control is on.");
		}
	}
	else
	{
		sendChannelError();
	}
}

void onCommandTest()
{
	double analogVoltage = atof(gParameters[0]);

	if (isDoubleWithinRange(analogVoltage, 0, 5))
	{
		int voltage = (int)(analogVoltage / 5.0 * 255);
		analogWrite(9, voltage);
		sendAck();
	}
	else
	{
		sendDoubleRangeError(0, 5, VOLTAGE_UNIT);
	}
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

	send(tmpstr);
	sendNack();
}