/*
Name: controller.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _CONTROLLER_h
#define _CONTROLLER_h

#include "commandHandler.h"
#include "dac.h"
#include "adc.h"
#include "potentiometer.h"
#include "encoder.h"
#include "yaw.h"
#include "tilt.h"
#include "pidController.h"

class Controller
{
private:
	Dac dac;
	Adc adc;
	Potentiometer potentiometer;
	Encoder encoder;
	Yaw yaw;
	Tilt tilt;
	PidController pidController;
	char lineBuffer[Utility::INT_LineSizeMax];
	long heartbeatCounter = 0;

	void updateHeartbeat(void);
	void clearParameters(void);
	void processCommand(char *command);
	void processLine(char *str);
	void processParameters(char *remainingString);

public:
	Controller();
	~Controller() {}
	void initialize(void);
	void scanSerialPort(void);
};

#endif
