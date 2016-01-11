/*
Name: HelicopterController.ino
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include "controller.h"
#include "pidControl.h"

void setup()
{
	initializeController();
	initializePwm();
	initializePid();
}

void loop()
{
	scanSerialPort();
}