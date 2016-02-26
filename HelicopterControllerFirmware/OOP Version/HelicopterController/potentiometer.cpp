//
//
//

#include "potentiometer.h"

const double Potentiometer::DBL_TotalRangeDegrees = 290.0;
const double Potentiometer::DBL_DegreesPerVolt = DBL_TotalRangeDegrees / Utility::DBL_ArduinoVss;
const double Potentiometer::DBL_IdleVoltage = 2.5;

Potentiometer::Potentiometer()
{
	Adc adc;
}

double Potentiometer::convertAdcVoltageToAngle(double voltage)
{
	double angle = (voltage - DBL_IdleVoltage) * DBL_DegreesPerVolt;
	return angle;
}

double Potentiometer::getAngle(int channel)
{
	double voltage = adc.getSampledAdcVoltage(channel);
	double angle = convertAdcVoltageToAngle(voltage);

	return angle;
}

void Potentiometer::updateAngle(void)
{
	double angle = getAngle(channel);
	currentAngle = angle;
}