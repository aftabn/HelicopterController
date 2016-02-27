/*
Name: adc.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _ADC_h
#define _ADC_h

#include "Arduino.h"

class Adc
{
private:
	static const int INT_Resolution = 1023;
	static const int INT_ChannelMin = 0;
	static const int INT_ChannelMax = 7;
	static const int INT_DefaultNumSamples = 50;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const byte adcChannelLookup[];

	int getValue(int channel);
	double convertAdcValueToVoltage(int adcValue);

public:
	Adc() {}
	~Adc() {}

	void initialize(void);
	double getSampledVoltage(int channel);
	double getSampledVoltage(int channel, uint8_t numSamples);
	double getVoltage(int channel);
};

#endif
