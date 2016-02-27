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
	Yaw *yaw;
	Tilt *tilt;
	const double DBL_MotorIdleVoltage;
	bool isPidEnabled;
	double angleErrors[Utility::INT_MaxNumChannels];

public:
	PidController(Yaw *yaw, Tilt *tilt) : yaw(yaw), tilt(tilt), DBL_MotorIdleVoltage(2.5) {}
	~PidController() {}

	// Pid ISR loop interval in milliseconds
	int pidLoopInterval;

	void enablePid(void);
	void disablePid(void);
};

#endif
