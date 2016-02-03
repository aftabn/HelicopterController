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
	initializeSPI();
	initializeAdc(); // Must be initialized after SPI
	initializeDac(); // Must be initialized after SPI
	initializeFrequencyOutput();
	initializePid();
}

void loop()
{
	scanSerialPort();
}