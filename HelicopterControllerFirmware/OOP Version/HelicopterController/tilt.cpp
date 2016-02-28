/*
Name: tilt.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "motor.h"
#include "tilt.h"
#include "encoder.h"

using namespace Motor;

Tilt::Tilt(Dac *dac, Potentiometer *potentiometer)
	: dac(dac), potentiometer(potentiometer), INT_MinOutput(0), INT_MaxOutput(100)
{
	currentAngle = &(potentiometer->currentAngle);
}

Tilt::~Tilt()
{
	delete potentiometer;
}

void Tilt::refreshAngle()
{
	potentiometer->updateAngle();
}

void Tilt::applyMotorOutputs(Direction *newDirection, int *newOutput)
{
	if (motorDriverType == MotorDriverType::AnalogVoltage)
	{
		double voltage = adjustOutputToVoltage(newDirection, newOutput);
		dac->setVoltage(INT_MotorChannel, voltage);
	}
	else if (motorDriverType == MotorDriverType::Frequency)
	{
		int frequency = adjustOutputToFrequency(newOutput);
	}

	currentOutput = *newOutput;
	direction = *newDirection;
}

double Tilt::adjustOutputToVoltage(Direction *newDirection, int *newOutput)
{
	double voltage = Motor::DBL_MotorIdleVoltage;

	voltage += Motor::DBL_MotorVoltageRange * (*newOutput) / 100.0;
	voltage *= (*newDirection) == Direction::Clockwise ? 1 : -1;

	return voltage;
}

int Tilt::adjustOutputToFrequency(int *newOutput)
{
	int frequency = (Motor::INT_MotorMaxFrequency - Motor::INT_MotorMaxFrequency) * (*newOutput) / 100.0;
	return frequency;
}