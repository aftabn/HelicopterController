/*
Name: frequencyGenerator.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "Arduino.h"
#include <digitalWriteFast.h>
#include "frequencyGenerator.h"
#include "utility.h"
#include "motor.h"

FrequencyGenerator::FrequencyGenerator() {}

FrequencyGenerator::~FrequencyGenerator() {}

void FrequencyGenerator::initialize()
{
	pinModeFast(Utility::PIN_FrequencyOutput, OUTPUT);
	disable();
}

void FrequencyGenerator::setFrequency(int frequency)
{
	// Constrain the frequency between the limits
	frequency = max(min(frequency, Motor::INT_MotorMaxFrequency), Motor::INT_MotorMinFrequency);

	tone(Utility::PIN_FrequencyOutput, frequency);

	currentFrequency = frequency;
}

void FrequencyGenerator::disable()
{
	noTone(Utility::PIN_FrequencyOutput);
	currentFrequency = 0;
}