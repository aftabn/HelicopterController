// potentiometer.h

#ifndef _POTENTIOMETER_h
#define _POTENTIOMETER_h

#include "adc.h"

class Potentiometer
{
private:
	Adc adc;
	static const int channel = 0;
	static const double DBL_TotalRangeDegrees;
	static const double DBL_DegreesPerVolt;
	static const double DBL_IdleVoltage;

	double convertAdcVoltageToAngle(double voltage);
	double getAngle(int channel);

public:
	Potentiometer();
	~Potentiometer();

	double currentAngle;
	void initialize(void);
	void updateAngle(void);
};

#endif
