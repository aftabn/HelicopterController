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
#include "motorEnums.h"

class Yaw
{
private:
	Encoder *encoder;
	Dac *dac;
	static const byte INT_MotorChannel = 0;

public:
	Yaw(Dac *dac, Encoder *encoder) : dac(dac), encoder(encoder) {}
	~Yaw() {}

	PidSettings pidSettings;
	volatile double setPoint;
	volatile MotorEnums::Direction direction;
	volatile MotorEnums::MotorDriverType motorDriverType;

	void initialize(void);
	double getAngle(void);
};

#endif
