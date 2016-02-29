/*
Name: tilt.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _TILT_h
#define _TILT_h

#include "pidSettings.h"
#include "frequencyGenerator.h"
#include "potentiometer.h"
#include "dac.h"
#include "motor.h"

// TODO: Add a preprocessor if statement to define either a DAC or frequency output for
// controlling the motors (based on whether it's demo 1 or 2)
class Tilt
{
private:
	Dac *dac;
	FrequencyGenerator *frequencyGenerator;
	Potentiometer *potentiometer;

	double adjustOutputToVoltage(Motor::Direction *newDirection, int *newOutput);
	int adjustOutputToFrequency(int *newOutput);

public:
	Tilt(Dac *dac, FrequencyGenerator *frequencyGenerator, Potentiometer *potentiometer);
	~Tilt();

	static const byte INT_MotorChannel = 1;
	static const double DBL_SetPointMin;
	static const double DBL_SetPointMax;
	static const int INT_MinOutput;
	static const int INT_MaxOutput;
	PidSettings pidSettings;
	volatile double setPoint;
	volatile double *currentAngle;
	volatile double currentOutput;
	volatile Motor::Direction direction;
	volatile Motor::MotorDriverType motorDriverType;

	void initialize(void);
	void refreshAngle(void);
	void applyMotorOutputs(Motor::Direction *newDirection, int *newOutput);
};

#endif
