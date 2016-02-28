/*
Name: yaw.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "yaw.h"
#include "motor.h"

using namespace Motor;

Yaw::Yaw(Dac *dac, Encoder *encoder) : dac(dac), encoder(encoder), INT_MinOutput(-100), INT_MaxOutput(100)
{
	currentAngle = &(encoder->currentAngle);
}

Yaw::~Yaw()
{
	Encoder::destruct();
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