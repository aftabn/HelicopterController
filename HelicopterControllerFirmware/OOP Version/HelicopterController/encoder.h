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
	Encoder();
	~Encoder();
	static Encoder *encoder;
	static const byte INT_MotorChannel = 0;
	static const int INT_PulsesPerRevolution;
	static const double DBL_TotalRangeDegrees;
	static const double DBL_DegreesPerPulse;
	static const int encoderLookup[16]; // One for each possible state
	byte encoderValues;

	static void quadratureDecoderISR(void);

public:
	volatile double currentAngle;

	static Encoder *getEncoder(void);
	static void destruct(void);
	void initialize(void);
	void zeroAngle(void);
};

#endif
