/*
Name: decoder.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "decoder.h"

const double Decoder::DBL_TotalRangeDegrees = 360.0;
const double Decoder::DBL_DegreesPerPulse = Decoder::DBL_TotalRangeDegrees / Decoder::DBL_DegreesPerPulse;
const signed int Decoder::encoderLookup[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
}

void Decoder::initialize()
{
	pinModeFast(Utility::PIN_EncoderChannelA, INPUT);
	pinModeFast(Utility::PIN_EncoderChannelB, INPUT);
	encoderValues = (digitalReadFast(Utility::PIN_EncoderChannelA) << 1) | digitalReadFast(Utility::PIN_EncoderChannelB);

	// Sets ISR for external interrupt on pin 2
	attachInterrupt(0, quadratureDecoderISR, RISING);
}

// Taken and modified from: http://www.mkesc.co.uk/ise.pdf
void Decoder::quadratureDecoderISR(void)
{
	encoderValues <<= 2;
	encoderValues |= ((digitalReadFast(Utility::PIN_EncoderChannelA) << 1) | digitalReadFast(Utility::PIN_EncoderChannelB));
	_currentAngle += encoderLookup[encoderValues & 0x0F] * Decoder::DBL_DegreesPerPulse;
}