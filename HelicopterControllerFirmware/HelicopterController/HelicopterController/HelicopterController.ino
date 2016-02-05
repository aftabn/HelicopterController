/*
Name: HelicopterController.ino
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <SPI.h>
#include "controller.h"
#include "pidControl.h"

void setup()
{
	initializeController();
	initializeSpi();
	initializeAdc(); // Must be initialized after SPI
	initializeDac(); // Must be initialized after SPI
	initializeFrequencyOutput();
	initializeQuadratureDecoder();
	initializePotentiometerTimer();
	initializePid();
}

void loop()
{
	scanSerialPort();
}