/*
Name: pidController.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include <Streaming.h>
#include "pidController.h"
#include "utility.h"
#include "motor.h"

using namespace Motor;

PidController *PidController::pidController;

PidController::PidController(Yaw *yaw, Tilt *tilt)
	: yaw(yaw), tilt(tilt), DBL_MotorIdleVoltage(2.5), DBL_DefaultPidInterval(250) {}

PidController::~PidController()
{
	delete yaw;
	delete tilt;
}

PidController* PidController::getPidController(Yaw *yaw, Tilt *tilt)
{
	if (pidController == nullptr)
	{
		pidController = new PidController(yaw, tilt);
	}

	return pidController;
}

PidController* PidController::getPidController()
{
	return pidController;
}

// Sets up the pointers to all the motors pid values and other variables
void PidController::initialize()
{
	pidLoopInterval = DBL_DefaultPidInterval;

	// Set up yaw constants
	pGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.proportionalGain);
	iGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.integralGain);
	dGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.derivativeGain);
	iWindupThresholds[Yaw::INT_MotorChannel] = &(yaw->pidSettings.integralWindupThreshold);
	outputRateLimits[Yaw::INT_MotorChannel] = &(yaw->pidSettings.outputRateLimit);
	setPoints[Yaw::INT_MotorChannel] = &(yaw->setPoint);
	currentAngles[Yaw::INT_MotorChannel] = yaw->currentAngle;
	directions[Yaw::INT_MotorChannel] = &(yaw->direction);
	motorDriverTypes[Yaw::INT_MotorChannel] = &(yaw->motorDriverType);
	minMotorOutputs[Yaw::INT_MotorChannel] = &(yaw->INT_MinOutput);
	maxMotorOutputs[Yaw::INT_MotorChannel] = &(yaw->INT_MaxOutput);

	// Set up tilt constants
	pGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.proportionalGain);
	iGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.integralGain);
	dGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.derivativeGain);
	iWindupThresholds[Tilt::INT_MotorChannel] = &(tilt->pidSettings.integralWindupThreshold);
	outputRateLimits[Tilt::INT_MotorChannel] = &(tilt->pidSettings.outputRateLimit);
	setPoints[Tilt::INT_MotorChannel] = &(tilt->setPoint);
	currentAngles[Tilt::INT_MotorChannel] = tilt->currentAngle;
	directions[Tilt::INT_MotorChannel] = &(tilt->direction);
	motorDriverTypes[Tilt::INT_MotorChannel] = &(tilt->motorDriverType);
	minMotorOutputs[Tilt::INT_MotorChannel] = &(tilt->INT_MinOutput);
	maxMotorOutputs[Tilt::INT_MotorChannel] = &(tilt->INT_MaxOutput);
}

void PidController::executePidCalculation()
{
	for (int channel = 0; channel < Utility::INT_MaxNumChannels; channel++)
	{
		Direction newDirection;
		int newOutput;

		// Updates the direction and the output
		updatePidMotorOutputs(channel, &newDirection, &newOutput);

		// Makes the necessary hardware output changes based on driver type
		applyMotorOutputs(channel, &newDirection, &newOutput);

		if (isVerboseMode)
		{
			// Format is: "[CH%d] SP: %s deg, Output: %d %%, Angle: %s deg"
			Serial << F("[CH") << channel << F("] SP: ") << (*setPoints[channel]) << F("deg, Output: ")
				<< (*currentOutputs[channel]) << F("%, Angle: ") << (*currentAngles[channel]) << F(" degs")
				<< Utility::STR_NewLine;
		}
	}
}

void PidController::updatePidMotorOutputs(int channel, Motor::Direction* direction, int* percentageOutput)
{
	// Get output as a signed value from -100 to 100 based on direction for easier calculations
	int currentOutput = (*currentOutputs[channel]);
	currentOutput *= (*directions[channel]) == Direction::Clockwise ? 1 : -1;

	double currentAngle = *currentAngles[channel];

	// P term
	angleErrors[channel] = (*setPoints[channel]) - currentAngle;

	// I term
	if (abs(angleErrors[channel]) < *iWindupThresholds[channel])
	{
		integratedAngleErrors[channel] = integratedAngleErrors[channel] + angleErrors[channel] * pidLoopInterval / 1000.0;
	}

	// D term
	derivativeAnglesErrors[channel] = (currentAngle - previousAngles[channel]) / pidLoopInterval / 1000.0;

	// Update previous angle for next calculation
	previousAngles[channel] = currentAngle;

	// Get new signed output from PID algorithm
	int newOutput = (int)((*pGains[channel]) * angleErrors[channel] + (*iGains[channel]) * integratedAngleErrors[channel] + (*dGains[channel]) * derivativeAnglesErrors[channel]);

	// Limit the amount the output can change by
	if (abs(newOutput - currentOutput) > (*outputRateLimits[channel]))
	{
		if (newOutput > currentOutput)
		{
			newOutput = currentOutput + (*outputRateLimits[channel]);
		}
		else
		{
			newOutput = currentOutput - (*outputRateLimits[channel]);
		}
	}

	// Constrain the output between the upper and lower bounds
	newOutput = max(min(newOutput, *maxMotorOutputs[channel]), *minMotorOutputs[channel]);

	// Apply the changes and switch output back to between 0 and 100 %
	*percentageOutput = abs(newOutput);
	*direction = newOutput > 0 ? Direction::Clockwise : Direction::CounterClockwise;
}

void PidController::applyMotorOutputs(int channel, Direction *direction, int *percentageOutput)
{
	if (channel == Yaw::INT_MotorChannel)
	{
		yaw->applyMotorOutputs(direction, percentageOutput);
	}
	else if (channel == Tilt::INT_MotorChannel)
	{
		tilt->applyMotorOutputs(direction, percentageOutput);
	}
}

// This is the ISR that runs the PID algorithm
ISR(TIMER1_COMPA_vect)
{
	PidController *pidController = PidController::getPidController();
	if (pidController->isPidEnabled)
	{
		pidController->isPidCalculationNeeded = true;
	}
}