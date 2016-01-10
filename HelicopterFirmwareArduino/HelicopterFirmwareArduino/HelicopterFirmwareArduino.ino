/*
 Name:		HelicopterFirmwareArduino.ino
 Created:	1/10/2016 1:09:10 PM
 Author:	Aftab
*/

#include "util.h"
#include "controller.h"

void setup()
{
	initController();
}

void loop()
{
	scanUSBPort();
}