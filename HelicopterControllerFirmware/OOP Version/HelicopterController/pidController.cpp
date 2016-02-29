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

const double PidController::DBL_DefaultPidInterval = 250;
const double PidController::DBL_MotorIdleVoltage = 2.5;

const int PidController::INT_MinPidOutput = 0;
const int PidController::INT_MaxPidOutput = 100;
const int PidController::INT_MinPidLoopInterval = 1;
const int PidController::INT_MaxPidLoopInterval = 260;

PidController *PidController::pidController;

PidController::PidController(Yaw *yaw, Tilt *tilt) : yaw(yaw), tilt(tilt) {}

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

// Sets up the pointers to all the motor's pid values and other member variables
void PidController::initialize()
{
	pidLoopInterval = DBL_DefaultPidInterval;

	// Set up yaw constants
	pGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.proportionalGain);
	iGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.integralGain);
	dGains[Yaw::INT_MotorChannel] = &(yaw->pidSettings.derivativeGain);
	integralWindupThresholds[Yaw::INT_MotorChannel] = &(yaw->pidSettings.integralWindupThreshold);
	outputRateLimits[Yaw::INT_MotorChannel] = &(yaw->pidSettings.outputRateLimit);
	setPoints[Yaw::INT_MotorChannel] = &(yaw->setPoint);
	currentAngles[Yaw::INT_MotorChannel] = yaw->currentAngle;
	directions[Yaw::INT_MotorChannel] = &(yaw->direction);
	motorDriverTypes[Yaw::INT_MotorChannel] = &(yaw->motorDriverType);
	minMotorOutputs[Yaw::INT_MotorChannel] = &(Yaw::INT_MinOutput);
	maxMotorOutputs[Yaw::INT_MotorChannel] = &(Yaw::INT_MaxOutput);
	minSetPoints[Yaw::INT_MotorChannel] = &(Yaw::DBL_SetPointMin);
	maxSetPoints[Yaw::INT_MotorChannel] = &(Yaw::DBL_SetPointMax);

	// Set up tilt constants
	pGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.proportionalGain);
	iGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.integralGain);
	dGains[Tilt::INT_MotorChannel] = &(tilt->pidSettings.derivativeGain);
	integralWindupThresholds[Tilt::INT_MotorChannel] = &(tilt->pidSettings.integralWindupThreshold);
	outputRateLimits[Tilt::INT_MotorChannel] = &(tilt->pidSettings.outputRateLimit);
	setPoints[Tilt::INT_MotorChannel] = &(tilt->setPoint);
	currentAngles[Tilt::INT_MotorChannel] = tilt->currentAngle;
	directions[Tilt::INT_MotorChannel] = &(tilt->direction);
	motorDriverTypes[Tilt::INT_MotorChannel] = &(tilt->motorDriverType);
	minMotorOutputs[Tilt::INT_MotorChannel] = &(Tilt::INT_MinOutput);
	maxMotorOutputs[Tilt::INT_MotorChannel] = &(Tilt::INT_MaxOutput);
	minSetPoints[Tilt::INT_MotorChannel] = &(Tilt::DBL_SetPointMin);
	maxSetPoints[Tilt::INT_MotorChannel] = &(Tilt::DBL_SetPointMax);
}

// Initializes the PID Timer (Timer 1) with the appropriate interval in milliseconds
void PidController::initializePidTimer(int numMilliseconds)
{
	noInterrupts();							// disable all interrupts
	TCCR1A = 0;								// set entire TCCR1A register to 0
	TCCR1B = 0;								// same for TCCR1B
	TCNT1 = 0;								// initialize counter value to 0
	OCR1A = 250 * numMilliseconds - 1;		// compare match register (16MHz/64/1000 = 250) * numMilliseconds - 1
	TCCR1B |= (1 << WGM12);					// CTC mode
	TCCR1B |= (1 << CS11) | (1 << CS10);	// 64 prescaler
	TIMSK1 |= (1 << OCIE1A);				// enable timer overflow interrupt
	interrupts();							// enable all interrupts
}

void PidController::enablePid()
{
	resetPidValues();
	initializePidTimer(pidLoopInterval);
	isPidEnabled = true;
}

void PidController::disablePid()
{
	isPidEnabled = false;
	disableMotors();
}

void PidController::resetPidValues()
{
	for (byte channel = 0; channel < Utility::INT_MaxNumChannels; channel++)
	{
		previousAngles[channel] = 0;
		angleErrors[channel] = 0;
		integratedAngleErrors[channel] = 0;
		derivativeAnglesErrors[channel] = 0;
	}
}

void PidController::disableMotors()
{
	yaw->disable();
	tilt->disable();
}

void PidController::executePidCalculation()
{
	for (byte channel = 0; channel < Utility::INT_MaxNumChannels; channel++)
	{
		Direction newDirection;
		int newOutput;

		// Updates the direction and the output
		updateOutputsFromPid(channel, &newDirection, &newOutput);

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

// Gets the new calculated direction and percentage output for the specified motor channel
void PidController::updateOutputsFromPid(byte channel, Motor::Direction *newDirection, int *newOutput)
{
	// Update previous angle for next calculation
	previousAngles[channel] = *currentAngles[channel];

	int output = *currentOutputs[channel];

	convertToSignedOutput(&output, (Direction *)directions[channel]);
	calculateNewPidOutput(channel, &output);
	constrainOutput(channel, &output);
	convertToUnsignedOutput(&output, newDirection); // The new direction is updated here

	*newOutput = output;
}

// Applies the output and direction passed in to the specified motor channel
void PidController::applyMotorOutputs(byte channel, Direction *direction, int *percentageOutput)
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

// Get output as a signed value from -100 to 100 based on direction for easier calculations
void PidController::convertToSignedOutput(int *output, Direction *direction)
{
	*output *= *direction == Direction::Clockwise ? 1 : -1;
}

// Convert output back to between 0 and 100 %
void PidController::convertToUnsignedOutput(int *output, Direction *direction)
{
	*output = abs(*output);
	*direction = *output > 0 ? Direction::Clockwise : Direction::CounterClockwise;
}

// Calculates the new output based on the pid values
void PidController::calculateNewPidOutput(byte channel, int *newOutput)
{
	// P term
	angleErrors[channel] = *setPoints[channel] - *currentAngles[channel];

	// I term
	if (abs(angleErrors[channel]) < *integralWindupThresholds[channel])
	{
		integratedAngleErrors[channel] = integratedAngleErrors[channel] + angleErrors[channel] * pidLoopInterval / 1000.0;
	}

	// D term
	derivativeAnglesErrors[channel] = (*currentAngles[channel] - previousAngles[channel]) / pidLoopInterval / 1000.0;

	// Get new signed output from PID algorithm
	*newOutput = (int)((*pGains[channel]) * angleErrors[channel] + (*iGains[channel]) * integratedAngleErrors[channel] + (*dGains[channel]) * derivativeAnglesErrors[channel]);
}

// Constrains the output based on rate limit and max allowable percentage value
void PidController::constrainOutput(byte channel, int *newOutput)
{
	// Limit the amount the output can change by
	if (abs(*newOutput - *currentOutputs[channel]) > *outputRateLimits[channel])
	{
		if (*newOutput > *currentOutputs[channel])
		{
			*newOutput = *currentOutputs[channel] + *outputRateLimits[channel];
		}
		else
		{
			*newOutput = *currentOutputs[channel] - *outputRateLimits[channel];
		}
	}

	// Constrain the output between the upper and lower bounds
	*newOutput = max(min(*newOutput, *maxMotorOutputs[channel]), *minMotorOutputs[channel]);
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