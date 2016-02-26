/*
Name: potentiometer.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _POTENTIOMETER_h
#define _POTENTIOMETER_h

#include "adc.h"

class Potentiometer
{
private:
	Adc adc;
	static const byte INT_MotorChannel = 1;
	static const double DBL_TotalRangeDegrees;
	static const double DBL_DegreesPerVolt;
	const double DBL_IdleVoltage;

	double convertAdcVoltageToAngle(double voltage);
	double getAngle(byte channel);

public:
	Potentiometer(double idleVoltage);
	~Potentiometer();

	double currentAngle;

	void initialize(void);
	void updateAngle(void);
};

#endif
