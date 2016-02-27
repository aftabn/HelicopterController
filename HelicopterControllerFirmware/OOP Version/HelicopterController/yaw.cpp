/*
Name: yaw.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "yaw.h"

Yaw::Yaw(Dac *dac, Encoder *encoder) : dac(dac), encoder(encoder)
{
	currentAngle = &(encoder->currentAngle);
}

Yaw::~Yaw() {}