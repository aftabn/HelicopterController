// potentiometer.h

#ifndef _POTENTIOMETER_h
#define _POTENTIOMETER_h

#include "adc.h"

class Potentiometer
{
private:
	Potentiometer() {};
	Adc adc;

public:
	void initialize(void);
	double getSampledAdcVoltage(int channel);
	double getSampledAdcVoltage(int channel, uint8_t numSamples);
	int getAdcValue(int channel);
	double getAdcVoltage(int channel);
	double convertAdcValueToVoltage(int adcValue);
};

#endif
