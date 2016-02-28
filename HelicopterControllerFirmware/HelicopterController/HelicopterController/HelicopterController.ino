/*
Name: HelicopterController.ino
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <Streaming.h>
#include <digitalWriteFast.h>
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
	initializePid();
}

void loop()
{
	scanSerialPort();
}