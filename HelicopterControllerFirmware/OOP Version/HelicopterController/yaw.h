/*
Name: yaw.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _YAW_h
#define _YAW_h

#include "dac.h"
#include "encoder.h"
#include "pidSettings.h"
#include "motor.h"

class Yaw
{
private:
	Encoder *encoder;
	Dac *dac;

	double adjustOutputToVoltage(Motor::Direction *newDirection, int *newOutput);

public:
	Yaw(Dac *dac, Encoder *encoder);
	~Yaw();

	static const byte INT_MotorChannel = 0;
	const int INT_MinOutput;
	const int INT_MaxOutput;
	PidSettings pidSettings;
	volatile double setPoint;
	volatile double *currentAngle;
	volatile double currentOutput;
	volatile Motor::Direction direction;
	volatile Motor::MotorDriverType motorDriverType;

	void initialize(void);
	void applyMotorOutputs(Motor::Direction *newDirection, int *newOutput);
};

#endif
