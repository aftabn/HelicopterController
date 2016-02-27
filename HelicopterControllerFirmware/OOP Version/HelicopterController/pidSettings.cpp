/*
Name: pidSettings.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "pidSettings.h"

const double PidSettings::DBL_DefaultProportionalGain = 0.01;
const double PidSettings::DBL_DefaultIntegralGain = 0;
const double PidSettings::DBL_DefaultDerivativeGain = 0;
const double PidSettings::DBL_DefaultIntegralWindupThreshold = 30.0;
const int PidSettings::INT_DefaultOutputRateLimit = 10;

PidSettings::PidSettings()
{
	proportionalGain = DBL_DefaultProportionalGain;
	integralGain = DBL_DefaultIntegralGain;
	derivativeGain = DBL_DefaultDerivativeGain;
	integralWindupThreshold = DBL_DefaultIntegralWindupThreshold;
	outputRateLimit = INT_DefaultOutputRateLimit;
}