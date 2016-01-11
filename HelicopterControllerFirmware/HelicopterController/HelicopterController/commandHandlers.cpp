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
	double newPGain = atof(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendDouble(pGain, DEFAULT_NUM_DECIMALS);
		sendAck();
	}
	else if (isDoubleWithinRange(newPGain, P_GAIN_MIN, P_GAIN_MAX))
	{
		pGain = newPGain;
		sendAck();
	}
	else
	{
		sendDoubleRangeError(P_GAIN_MIN, P_GAIN_MAX, NO_UNIT);
	}
}

void onCommandIntegralGain()
{
	double newIGain = atof(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendDouble(iGain, DEFAULT_NUM_DECIMALS);
		sendAck();
	}
	else if (isDoubleWithinRange(newIGain, I_GAIN_MIN, I_GAIN_MAX))
	{
		iGain = newIGain;
		sendAck();
	}
	else
	{
		sendDoubleRangeError(I_GAIN_MIN, I_GAIN_MAX, NO_UNIT);
	}
}

void onCommandDerivativeGain()
{
	double newDGain = atof(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendDouble(dGain, DEFAULT_NUM_DECIMALS);
		sendAck();
	}
	else if (isDoubleWithinRange(newDGain, D_GAIN_MIN, D_GAIN_MAX))
	{
		dGain = newDGain;
		sendAck();
	}
	else
	{
		sendDoubleRangeError(D_GAIN_MIN, D_GAIN_MAX, NO_UNIT);
	}
}

void onCommandLoopInterval()
{
	int newLoopInterval = convertToInt(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendInt(loopInterval);
		sendAck();
	}
	else if (isIntWithinRange(newLoopInterval, PID_INTERVAL_SEC_MIN, PID_INTERVAL_SEC_MAX))
	{
		loopInterval = newLoopInterval;
		sendAck();
	}
	else
	{
		sendIntRangeError(PID_INTERVAL_SEC_MIN, PID_INTERVAL_SEC_MAX, MILLISECONDS_UNIT);
	}
}

void onCommandSetPoint()
{
	double newSetPoint = atof(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendDouble(setPoint, DEFAULT_NUM_DECIMALS);
		sendAck();
	}
	else if (isDoubleWithinRange(newSetPoint, SET_POINT_MIN, SET_POINT_MAX))
	{
		setPoint = newSetPoint;
		sendAck();
	}
	else
	{
		sendDoubleRangeError(SET_POINT_MIN, SET_POINT_MAX, DEGREES_UNIT);
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
	int duty = convertToInt(gParameters[0]);

	if (isReadCommand(gParameters[0]))
	{
		sendInt(currentDutyCycle);
		sendAck();
	}
	else if (!isPidEnabled)
	{
		if (isIntWithinRange(duty, PWM_DUTY_MIN, PWM_DUTY_MAX))
		{
			setPwmDutyCycle(duty);
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

void onCommandTest()
{
	double a = 15.0;
	double b = 1.5;

	sendDouble(a / b, DEFAULT_NUM_DECIMALS);
}

void onCommandHelp()
{
	send("Command: *IDN? \r\nDescription: Returns the identity of the controller\r\n");
	send("Command: SYS \r\nArg: VER \r\nDescription: Gets information about jig or the controller\r\n");
	send("Command: ECHO \r\nArg: Any string \r\nDescription: Returns entered string\r\n");
	send("Command: P \r\nArg: None or Value\r\nDescription: Gets or sets PGain for selected channel's PID control loop\r\n");
	send("Command: I \r\nArg: None or Value\r\nDescription: Gets or sets IGain for selected channel's PID control loop\r\n");
	send("Command: D \r\nArg: None or Value\r\nDescription: Gets or sets DGain for selected channel's PID control loop\r\n");
	send("Command: INTERVAL \r\nArg: None or Value in milliseconds\r\nDescription: Gets or sets interval for selected channel's PID control loop\r\n");
	send("Command: DUTY \r\nArg: None or Value (0 or 1)\r\nDescription: Gets or sets the value for the selected channel's duty cycle\r\n");
	send("Command: MAXDUTY \r\nArg1: None or Value (0 to 100)\r\nDescription: Gets or sets the maximum duty cycle\r\n");
	send("Command: SP \r\nArg: None or Value in degrees\r\nDescription: Gets or sets the set point temperature for the designated heater\r\n");
	send("Command: PID \r\nArg: ON or OFF \r\nDescription: Enables or disables PID loop control\r\n");
	send("Command: DEBUG \r\nArg: ON or OFF \r\nDescription: Enables or disables additional debugging info\r\n");
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