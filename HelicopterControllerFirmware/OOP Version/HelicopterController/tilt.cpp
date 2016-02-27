/*
Name: tilt.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "tilt.h"
#include "encoder.h"

Tilt::Tilt(Dac *dac, Potentiometer *potentiometer) : dac(dac), potentiometer(potentiometer)
{
	currentAngle = &(potentiometer->currentAngle);
}

Tilt::~Tilt() {}

void Tilt::refreshAngle()
{
	potentiometer->updateAngle();
}