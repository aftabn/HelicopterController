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
	static const int INT_Resolution;
	static const int INT_DefaultNumSamples;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const byte adcChannelLookup[];

	int getValue(byte channel);
	double convertAdcValueToVoltage(int adcValue);

public:
	Adc();
	~Adc();

	static const int INT_ChannelMin;
	static const int INT_ChannelMax;

	void initialize(void);
	double getSampledVoltage(byte channel);
	double getSampledVoltage(byte channel, uint8_t numSamples);
	double getVoltage(byte channel);
};

#endif
