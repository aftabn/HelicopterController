/*
Name: tilt.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _TILT_h
#define _TILT_h

#include "pidSettings.h"
#include "motorEnums.h"
#include "potentiometer.h"
#include "dac.h"
#include "motorEnums.h"

// TODO: Add a preprocessor if statement to define either a DAC or frequency output for
// controlling the motors (based on whether it's demo 1 or 2)
class Tilt
{
private:
	Potentiometer *potentiometer;
	Dac *dac;
	static const byte INT_MotorChannel = 1;

public:
	Tilt(Dac *dac, Potentiometer *potentiometer) : dac(dac), potentiometer(potentiometer) {}
	~Tilt() {}

	PidSettings pidSettings;
	volatile double setPoint;
	volatile MotorEnums::Direction direction;
	volatile MotorEnums::MotorDriverType motorDriverType;

	void initialize(void);
	double getAngle(void);
};

#endif
