/*
Name: pidSettings.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _PIDSETTINGS_h
#define _PIDSETTINGS_h

#include "Arduino.h"

class PidSettings
{
private:
	static const double DBL_DefaultProportionalGain;
	static const double DBL_DefaultIntegralGain;
	static const double DBL_DefaultDerivativeGain;
	static const double DBL_DefaultIntegralWindupThreshold;
	static const byte INT_DefaultOutputRateLimit;

public:
	PidSettings();
	~PidSettings();

	static const double DBL_ProportionalGainMin;
	static const double DBL_ProportionalGainMax;
	static const double DBL_IntegralGainMin;
	static const double DBL_IntegralGainMax;
	static const double DBL_DerivativeGainMin;
	static const double DBL_DerivativeGainMax;
	static const double DBL_IntegralWindupThresholdMin;
	static const double DBL_IntegralWindupThresholdMax;
	static const byte INT_OutputRateLimitMin;
	static const byte INT_OutputRateLimitMax;
	volatile double proportionalGain;
	volatile double integralGain;
	volatile double derivativeGain;
	volatile double integralWindupThreshold;
	volatile int outputRateLimit;
};

#endif
