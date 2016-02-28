/*
Name: pidSettings.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _PIDSETTINGS_h
#define _PIDSETTINGS_h

class PidSettings
{
private:
	static const double DBL_DefaultProportionalGain;
	static const double DBL_DefaultIntegralGain;
	static const double DBL_DefaultDerivativeGain;
	static const double DBL_DefaultIntegralWindupThreshold;
	static const int INT_DefaultOutputRateLimit;

public:
	PidSettings();
	~PidSettings();

	volatile double proportionalGain;
	volatile double integralGain;
	volatile double derivativeGain;
	volatile double integralWindupThreshold;
	volatile int outputRateLimit;
};

#endif
