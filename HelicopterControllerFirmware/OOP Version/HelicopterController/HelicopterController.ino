/*
Name: HelicopterController.ino
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <SPI.h>
#include <Streaming.h>
#include <digitalWriteFast.h>
#include "Arduino.h"
#include "controller.h"

Controller *controller;

void setup()
{
	controller = new Controller();
	controller->initialize();
}

void loop()
{
	controller->scanSerialPort();
}