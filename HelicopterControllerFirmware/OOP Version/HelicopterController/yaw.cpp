/*
Name: yaw.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "yaw.h"
#include "motor.h"

using namespace Motor;

const double Yaw::DBL_SetPointMin = -180;
const double Yaw::DBL_SetPointMax = 180;
const int Yaw::INT_MinOutput = -100;
const int Yaw::INT_MaxOutput = 100;

Yaw::Yaw(Dac *dac, Encoder *encoder) : dac(dac), encoder(encoder)
{
	currentAngle = &(encoder->currentAngle);
}

Yaw::~Yaw()
{
	Encoder::destruct();
}

void Yaw::initialize()
{
	setPoint = 0;
	currentOutput = 0;
	direction = Direction::Clockwise;
	motorDriverType = MotorDriverType::AnalogVoltage;
}

void Yaw::applyMotorOutputs(Direction *newDirection, int *newOutput)
{
	if (motorDriverType == MotorDriverType::AnalogVoltage)
	{
		double voltage = adjustOutputToVoltage(newDirection, newOutput);
		dac->setVoltage(INT_MotorChannel, voltage);
	}

	currentOutput = *newOutput;
	direction = *newDirection;
}

double Yaw::adjustOutputToVoltage(Direction *newDirection, int *newOutput)
{
	double voltage = Motor::DBL_MotorIdleVoltage;

	voltage += Motor::DBL_MotorVoltageRange * (*newOutput) / 100.0;
	voltage *= (*newDirection) == Direction::Clockwise ? 1 : -1;

	return voltage;
}