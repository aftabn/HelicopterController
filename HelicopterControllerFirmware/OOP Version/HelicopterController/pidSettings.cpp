/*
Name: pidSettings.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "Arduino.h"
#include "pidSettings.h"

const double PidSettings::DBL_DefaultProportionalGain = 0.01;
const double PidSettings::DBL_DefaultIntegralGain = 0;
const double PidSettings::DBL_DefaultDerivativeGain = 0;
const double PidSettings::DBL_DefaultIntegralWindupThreshold = 30.0;
const byte PidSettings::INT_DefaultOutputRateLimit = 30;

const double PidSettings::DBL_ProportionalGainMin = 0.01;
const double PidSettings::DBL_ProportionalGainMax = 10;
const double PidSettings::DBL_IntegralGainMin = 0;
const double PidSettings::DBL_IntegralGainMax = 10;
const double PidSettings::DBL_DerivativeGainMin = 0;
const double PidSettings::DBL_DerivativeGainMax = 10;
const double PidSettings::DBL_IntegralWindupThresholdMin = 0;
const double PidSettings::DBL_IntegralWindupThresholdMax = 100;
const byte PidSettings::INT_OutputRateLimitMin = 1;
const byte PidSettings::INT_OutputRateLimitMax = 50;

PidSettings::PidSettings()
{
	proportionalGain = DBL_DefaultProportionalGain;
	integralGain = DBL_DefaultIntegralGain;
	derivativeGain = DBL_DefaultDerivativeGain;
	integralWindupThreshold = DBL_DefaultIntegralWindupThreshold;
	outputRateLimit = INT_DefaultOutputRateLimit;
}

PidSettings::~PidSettings() {}