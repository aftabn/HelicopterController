/*
Name: dac.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _DAC_h
#define _DAC_h

#include "utility.h"

class Dac
{
private:
	static const int INT_Resolution = 1023;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const double DBL_DefaultVoltage;
	double currentVoltages[Utility::INT_MaxNumChannels];

	int convertVoltageToDacValue(double voltage);

public:
	Dac() {};
	~Dac() {};
	void initialize(void);
	void setVoltage(int channel, double voltage);
};

#endif
