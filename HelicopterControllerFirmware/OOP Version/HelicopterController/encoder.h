/*
Name: encoder.h
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _ENCODER_h
#define _ENCODER_h

#include "Arduino.h"

class Encoder
{
private:
	static const byte INT_MotorChannel = 0;
	static const int INT_PulsesPerRevolution = 400;
	static const double DBL_TotalRangeDegrees;
	static const double DBL_DegreesPerPulse;
	static const int encoderLookup[16]; // One for each possible state
	static byte encoderValues;
	static volatile double _currentAngle;
	static void quadratureDecoderISR(void);

public:
	Encoder() {}
	~Encoder() {}

	double currentAngle() { return _currentAngle; }

	void initialize(void);
	void zeroAngle(void);
};

#endif
