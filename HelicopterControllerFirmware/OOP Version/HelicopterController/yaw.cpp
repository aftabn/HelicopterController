/*
Name: yaw.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "yaw.h"

double Yaw::getAngle()
{
	return encoder->currentAngle();
}