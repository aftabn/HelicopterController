/*
Name: commandHandler.h
Created: 1/10/2016
Author:	Aftab
*/

#ifndef _COMMANDHANDLER_h
#define _COMMANDHANDLER_h

#include "utility.h"
#include "pidController.h"

class CommandHandler
{
private:
	CommandHandler() {}

public:
	static char gParameters[Utility::INT_ParameterCountMax][Utility::INT_ParameterLengthMax + 1];

	static void onCommandIdentity(void);
	static void onCommandVersion(void);
	static void onCommandChangelog(void);
	static void onCommandEcho(void);
	static void onCommandPidControl(PidController *pidController);
	/*static void onCommandVerbose(void);
	static void onCommandSafety(void);
	static void onCommandOutput(void);
	static void onCommandDirection(void);
	static void onCommandMotorDriver(void);
	static void onCommandProportionalGain(void);
	static void onCommandIntegralGain(void);
	static void onCommandDerivativeGain(void);
	static void onCommandLoopInterval(void);
	static void onCommandIntegralWindup(void);
	static void onCommandRateLimit(void);
	static void onCommandSetPoint(void);
	static void onCommandAngle(void);
	static void onCommandAdc(void);
	static void onCommandDacVoltage(void);
	static void onCommandFrequencyOutput(void);*/
	static void onCommandHelp(void);
	static void handleCommandUnknown(char *command);
};

#endif
