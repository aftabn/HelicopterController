/*
Name: tilt.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "tilt.h"

double Tilt::getAngle()
{
	potentiometer->updateAngle();
	return potentiometer->currentAngle;
}