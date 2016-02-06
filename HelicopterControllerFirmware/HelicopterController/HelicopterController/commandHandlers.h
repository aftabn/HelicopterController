/*
Name: commandHandlers.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _COMMANDHANDLERS_h
#define _COMMANDHANDLERS_h

#define NO_UNIT				""
#define PERCENTAGE_UNIT		"%"
#define MILLISECONDS_UNIT	"ms"
#define HERTZ_UNIT			"Hz"
#define DEGREES_UNIT		"degs"
#define VOLTAGE_UNIT		"V"

void onCommandIdentity(void);
void onCommandVersion(void);
void onCommandChangelog(void);
void onCommandEcho(void);
void onCommandPidControl(void);
void onCommandVerbose(void);
void onCommandSafety(void);
void onCommandOutput(void);
void onCommandDirection(void);
void onCommandMotorDriver(void);
void onCommandProportionalGain(void);
void onCommandIntegralGain(void);
void onCommandDerivativeGain(void);
void onCommandLoopInterval(void);
void onCommandIntegralWindup(void);
void onCommandSetPoint(void);
void onCommandAngle(void);
void onCommandAdc(void);
void onCommandDacVoltage(void);
void onCommandFrequencyOutput(void);
void onCommandTest(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

#endif
