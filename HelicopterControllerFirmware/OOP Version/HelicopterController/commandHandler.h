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
	static void onCommandFirmwareVersion(const double *firmwareVersion);
	static void onCommandChangelog(void);
	static void onCommandEcho(void);
	static void onCommandPidControl(PidController *pidController);
	static void onCommandVerbose(bool *isVerboseMode);
	static void onCommandSafety(bool *isSafetyOn);
	static void onCommandOutput(PidController *pidController);
	static void onCommandDirection(PidController *pidController);
	static void onCommandMotorDriver(PidController *pidController);
	static void onCommandProportionalGain(PidController *pidController);
	static void onCommandIntegralGain(PidController *pidController);
	static void onCommandDerivativeGain(PidController *pidController);
	static void onCommandLoopInterval(PidController *pidController);
	static void onCommandIntegralWindup(PidController *pidController);
	static void onCommandRateLimit(PidController *pidController);
	static void onCommandSetPoint(PidController *pidController);
	static void onCommandAngle(PidController *pidController);
	static void onCommandAdc(Adc *adc);
	static void onCommandDacVoltage(bool *isSafetyOn, Dac *dac);
	static void onCommandFrequencyOutput(bool *isSafetyOn, FrequencyGenerator *frequencyGenerator);
	static void onCommandHelp(void);
	static void handleCommandUnknown(char *command);

	static void sendNack(void);
	static void sendAck(void);
	static void sendInt(int num);
	static void sendDouble(double num);
	static void sendDouble(double num, int numDecimals);
	static void sendOnOffStatus(bool isOn);
	static void sendOneOrZeroStatus(bool isHigh);
	static void sendDirectionStatus(Motor::Direction direction);
	static void sendMotorDriverStatus(Motor::MotorDriverType motorDriverType);
	static void sendIntRangeError(int lowerLimit, int upperLimit, const char* unit);
	static void sendDoubleRangeError(double lowerLimit, double upperLimit, const char* unit);
	static void sendChannelError(void);
	static void sendSyntaxError(void);
	static void sendReadOnlyError(void);
	static void sendOnOffError(void);
	static void sendOneOrZeroError(void);
	static void sendDirectionError(void);
	static void sendMotorDriverError(void);

	static bool isOnCommandArg(char* arg);
	static bool isOffCommandArg(char* arg);
	static bool isClockwiseCommandArg(char* arg);
	static bool isCounterClockwiseCommandArg(char* arg);
	static bool isAnalogVoltageCommandArg(char* arg);
	static bool isFrequencyCommandArg(char* arg);
	static bool isChannelCorrect(char* channelArg);
	static bool isReadCommand(char* arg);
	static bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
	static bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);
};

#endif
