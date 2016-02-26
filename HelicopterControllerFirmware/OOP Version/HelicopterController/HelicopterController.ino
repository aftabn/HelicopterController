/*
Name: HelicopterController.ino
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include "potentiometer.h"
#include "adc.h"
#include "dac.h"
#include "tilt.h"
#include "Tilt.h"
#include "yaw.h"
#include "Arduino.h"
#include <digitalWriteFast.h>
#include "controller.h"

Controller controller;

void setup()
{
	controller.initialize();
}

void loop()
{
	//Serial.print("h");
	controller.scanSerialPort();
}