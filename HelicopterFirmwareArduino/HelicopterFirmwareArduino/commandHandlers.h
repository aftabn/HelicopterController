// commandHandlers.h

#ifndef _COMMANDHANDLERS_h
#define _COMMANDHANDLERS_h

#define NO_UNIT				""
#define MILLISECONDS_UNIT	"ms"
#define DEGREES_UNIT		"degs"

void onCommandEcho(void);
void onCommandIdentity(void);
void onCommandSystem(void);
void onCommandDebug(void);
void onCommandPidLoopControl(void);
void onCommandProportionalGain(void);
void onCommandIntegralGain(void);
void onCommandDerivativeGain(void);
void onCommandLoopInterval(void);
void onCommandSetPoint(void);
void onCommandDuty(void);
void onCommandMaxDuty(void);
void onCommandStatus(void);
void onCommandTest(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

#endif
