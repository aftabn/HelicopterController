/*
Name: pidController.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _PIDCONTROLLER_h
#define _PIDCONTROLLER_h

#include "Arduino.h"
#include "utility.h"
#include "dac.h"
#include "yaw.h"
#include "tilt.h"

class PidController
{
private:
	PidController(Yaw *yaw, Tilt *tilt);
	~PidController();

	static const double DBL_DefaultPidInterval;
	static const double DBL_MotorIdleVoltage;
	const int *minMotorOutputs[Utility::INT_MaxNumChannels];
	const int *maxMotorOutputs[Utility::INT_MaxNumChannels];
	static PidController *pidController;
	Yaw *yaw;
	Tilt *tilt;
	double angleErrors[Utility::INT_MaxNumChannels];
	double integratedAngleErrors[Utility::INT_MaxNumChannels];
	double derivativeAnglesErrors[Utility::INT_MaxNumChannels];

	void initializePidTimer(int numMilliseconds);
	void resetPidValues(void);
	void updateOutputsFromPid(byte channel, Motor::Direction *direction, int *percentageOutput);
	void convertToSignedOutput(int *output, Motor::Direction *direction);
	void convertToUnsignedOutput(int *output, Motor::Direction *direction);
	void constrainOutput(byte channelint, int *newOutput);
	void calculateNewPidOutput(byte channel, int *newOutput);

public:
	// These two are only used when trying to explicitly set the pid percentage output
	// i.e. When the output is being set from outside of the PID loop
	static const int INT_MinPidOutput;
	static const int INT_MaxPidOutput;
	static const int INT_MinPidLoopInterval;
	static const int INT_MaxPidLoopInterval;
	const double *minSetPoints[Utility::INT_MaxNumChannels];
	const double *maxSetPoints[Utility::INT_MaxNumChannels];
	volatile bool isVerboseMode;
	volatile bool isPidEnabled;
	volatile bool isPidCalculationNeeded;
	volatile int pidLoopInterval;
	volatile double *pGains[Utility::INT_MaxNumChannels];
	volatile double *iGains[Utility::INT_MaxNumChannels];
	volatile double *dGains[Utility::INT_MaxNumChannels];
	volatile double *integralWindupThresholds[Utility::INT_MaxNumChannels];
	volatile int *outputRateLimits[Utility::INT_MaxNumChannels];
	volatile double *setPoints[Utility::INT_MaxNumChannels];
	volatile double *currentAngles[Utility::INT_MaxNumChannels];
	volatile double previousAngles[Utility::INT_MaxNumChannels];
	volatile int *currentOutputs[Utility::INT_MaxNumChannels];
	volatile Motor::Direction *directions[Utility::INT_MaxNumChannels];
	volatile Motor::MotorDriverType *motorDriverTypes[Utility::INT_MaxNumChannels];

	static PidController *getPidController(Yaw *yaw, Tilt *tilt);
	static PidController *getPidController();
	static void destruct(void);
	void initialize(void);
	void enablePid(void);
	void disablePid(void);
	void disableMotors(void);
	void applyMotorOutputs(byte channel, Motor::Direction *direction, int *percentageOutput);
	void executePidCalculation(void);
};

#endif
