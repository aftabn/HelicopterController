/*
 Name:		HelicopterFirmwareArduino.ino
 Created:	1/10/2016 1:09:10 PM
 Author:	Aftab
*/

#include <stdio.h>
#include "util.h"
#include "controller.h"

int incomingByte;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	scanUSBPort();
}