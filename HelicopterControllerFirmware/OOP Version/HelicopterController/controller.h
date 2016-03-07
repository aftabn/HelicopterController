/*
Name: controller.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _CONTROLLER_h
#define _CONTROLLER_h

#include "commandHandler.h"
#include "utility.h"

class Controller
{
private:
	static const double DBL_FirmwareVersion;

	// The controller needs to have access to all of these objects in order to pass them
	// as references to the commandhandler functions
	Dac *dac;
	Adc *adc;
	FrequencyGenerator *frequencyGenerator;
	Potentiometer *potentiometer;
	Encoder *encoder;
	Yaw *yaw;
	Tilt *tilt;
	PidController *pidController;
	char lineBuffer[Utility::INT_LineSizeMax];
	uint16_t heartbeatCounter = 0;
	bool isSafetyOn;
	bool *isVerboseMode;

	void initializeSpi(void);
	void updateHeartbeat(void);
	void clearParameters(void);
	void processCommand(char *command);
	void processLine(char *str);
	void processParameters(char *remainingString);

public:
	Controller();
	~Controller();

	void initialize(void);
	void scanSerialPort(void);
};

#endif
