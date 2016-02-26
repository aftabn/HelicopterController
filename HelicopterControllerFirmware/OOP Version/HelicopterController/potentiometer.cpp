/*
Name: potentiometer.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "potentiometer.h"

const double Potentiometer::DBL_TotalRangeDegrees = 290.0;
const double Potentiometer::DBL_DegreesPerVolt = DBL_TotalRangeDegrees / Utility::DBL_ArduinoVss;

Potentiometer::Potentiometer(double idleVoltage) : DBL_IdleVoltage(idleVoltage)
{
	Adc adc;
}

Potentiometer::~Potentiometer()
{
}

double Potentiometer::convertAdcVoltageToAngle(double voltage)
{
	double angle = (voltage - DBL_IdleVoltage) * DBL_DegreesPerVolt;
	return angle;
}

double Potentiometer::getAngle(byte channel)
{
	double voltage = adc.getSampledVoltage(channel);
	double angle = convertAdcVoltageToAngle(voltage);

	return angle;
}

void Potentiometer::updateAngle(void)
{
	double angle = getAngle(INT_MotorChannel);
	currentAngle = angle;
}