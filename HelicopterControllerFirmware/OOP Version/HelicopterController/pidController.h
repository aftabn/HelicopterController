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

	const double DBL_DefaultPidInterval;
	const double DBL_MotorIdleVoltage;
	static PidController *pidController;
	Yaw *yaw;
	Tilt *tilt;
	double angleErrors[Utility::INT_MaxNumChannels];
	double integratedAngleErrors[Utility::INT_MaxNumChannels];
	double derivativeAnglesErrors[Utility::INT_MaxNumChannels];
	const int *minMotorOutputs[Utility::INT_MaxNumChannels];
	const int *maxMotorOutputs[Utility::INT_MaxNumChannels];

	void updatePidMotorOutputs(int channel, Motor::Direction *direction, int *percentageOutput);
	void applyMotorOutputs(int channel, Motor::Direction *direction, int *percentageOutput);

public:
	// Pid ISR loop interval in milliseconds
	volatile bool isVerboseMode;
	volatile bool isPidEnabled;
	volatile bool isPidCalculationNeeded;
	volatile int pidLoopInterval;
	volatile double *pGains[Utility::INT_MaxNumChannels];
	volatile double *iGains[Utility::INT_MaxNumChannels];
	volatile double *dGains[Utility::INT_MaxNumChannels];
	volatile double *iWindupThresholds[Utility::INT_MaxNumChannels];
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
	void executePidCalculation(void);
};

#endif
