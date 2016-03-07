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
	static const int INT_Resolution;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const double DBL_DefaultVoltage;

	int convertVoltageToDacValue(double voltage);

public:
	Dac();
	~Dac();

	double currentVoltages[Utility::INT_MaxNumChannels];

	void initialize(void);
	void setVoltage(byte channel, double voltage);
};

#endif
