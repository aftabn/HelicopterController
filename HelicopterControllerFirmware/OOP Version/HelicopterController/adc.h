// adc.h

#ifndef _ADC_h
#define _ADC_h

class Adc
{
private:
	static const int INT_Resolution = 1023;
	static const int INT_ChannelMin = 0;
	static const int INT_ChannelMin = 7;
	static const int INT_DefaultNumSamples = 50;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const byte adcChannelLookup[];

	int getAdcValue(int channel);
	double convertAdcValueToVoltage(int adcValue);

public:
	Adc() {}
	void initialize(void);
	double getSampledAdcVoltage(int channel);
	double getSampledAdcVoltage(int channel, uint8_t numSamples);
	double getAdcVoltage(int channel);
};

#endif
