/*
Name: tilt.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "motor.h"
#include "tilt.h"
#include "encoder.h"

using namespace Motor;

const double Tilt::DBL_SetPointMin = 0;
const double Tilt::DBL_SetPointMax = 45;
const int Tilt::INT_MinOutput = 0;
const int Tilt::INT_MaxOutput = 100;

Tilt::Tilt(Dac *dac, FrequencyGenerator *frequencyGenerator, Potentiometer *potentiometer)
	: dac(dac), frequencyGenerator(frequencyGenerator), potentiometer(potentiometer)
{
	currentAngle = &(potentiometer->currentAngle);
}

Tilt::~Tilt()
{
	delete potentiometer;
	delete frequencyGenerator;
}

void Tilt::initialize()
{
	setPoint = 0;
	direction = Direction::Clockwise;
	motorDriverType = MotorDriverType::AnalogVoltage;
	disable();
}

// Disables both the frequency generator and dac
void Tilt::disable()
{
	dac->setVoltage(INT_MotorChannel, Motor::DBL_MotorIdleVoltage);
	frequencyGenerator->disable();
	currentOutput = 0;
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
		frequencyGenerator->setFrequency(frequency);
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