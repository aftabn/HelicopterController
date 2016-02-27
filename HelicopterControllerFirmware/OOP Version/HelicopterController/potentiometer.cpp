/*
Name: potentiometer.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "potentiometer.h"
#include "utility.h"

const double Potentiometer::DBL_TotalRangeDegrees = 290.0;
const double Potentiometer::DBL_DegreesPerVolt = DBL_TotalRangeDegrees / Utility::DBL_ArduinoVss;
const double Potentiometer::DBL_ZeroedVoltage = 2.5;

double Potentiometer::convertAdcVoltageToAngle(double voltage)
{
	double angle = (voltage - DBL_ZeroedVoltage) * DBL_DegreesPerVolt;
	return angle;
}

double Potentiometer::getAngle(byte channel)
{
	double voltage = adc->getSampledVoltage(channel);
	double angle = convertAdcVoltageToAngle(voltage);

	return angle;
}

void Potentiometer::updateAngle(void)
{
	double angle = getAngle(INT_MotorChannel);
	currentAngle = angle;
}