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
void onCommandSystem(void);
void onCommandEcho(void);
void onCommandPidControl(void);
void onCommandDebug(void);
void onCommandSafety(void);
void onCommandOutput(void);
void onCommandDirection(void);
void onCommandMotorDriver(void);
void onCommandProportionalGain(void);
void onCommandIntegralGain(void);
void onCommandDerivativeGain(void);
void onCommandLoopInterval(void);
void onCommandSetPoint(void);
void onCommandAdc(void);
void onCommandDacVoltage(void);
void onCommandFrequencyOutput(void);
void onCommandState(void);
void onCommandTest(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

#endif
