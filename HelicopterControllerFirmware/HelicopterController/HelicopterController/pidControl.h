/*
Name: pidControl.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _PIDCONTROL_h
#define _PIDCONTROL_h

#include "arduino.h"
#include "globals.h"

#define DAC_SLAVE_SELECT			10

#define P_GAIN_MIN					0.01
#define P_GAIN_MAX					10.0

#define I_GAIN_MIN					0.0
#define I_GAIN_MAX					20.0
#define I_GAIN_THRESHHOLD_ERROR		10

#define D_GAIN_MIN					0.0
#define D_GAIN_MAX					10.0

#define PWM_DUTY_INTERVALS			2
#define PWM_FREQUENCY				1

#define PWM_DUTY_MIN				0
#define PWM_DUTY_MAX				100

// For the current driver:
#define MOTOR_IDLE_VOLTAGE			2.5		// Idle voltage
#define MOTOR_MAX_VOLTAGE			5.0		// Full CW rotation
#define MOTOR_MIN_VOLTAGE			0.0		// Full CCW rotation

#define SET_POINT_MIN				0.0
#define SET_POINT_MAX				360.0

#define PID_INTERVAL_SEC_MIN		0.01
#define PID_INTERVAL_SEC_MAX		1.00

#define DEFAULT_P_GAIN				P_GAIN_MIN
#define DEFAULT_I_GAIN				I_GAIN_MIN
#define DEFAULT_D_GAIN				D_GAIN_MIN
#define DEFAULT_SET_POINT			SET_POINT_MIN
#define DEFAULT_PID_INTERVAL_SEC	PID_INTERVAL_SEC_MIN

extern volatile bool isPidEnabled;
extern volatile bool isDebugMode;

extern volatile double pGains[MAX_NUM_CHANNELS];
extern volatile double iGains[MAX_NUM_CHANNELS];
extern volatile double dGains[MAX_NUM_CHANNELS];
extern volatile double loopIntervals[MAX_NUM_CHANNELS];
extern volatile double setPoints[MAX_NUM_CHANNELS];
extern volatile uint8_t currentDuties[MAX_NUM_CHANNELS];

int getNewPidDutyCycle(int channel);
int adjustDutyToAllowableValue(int duty);
void setPwmDutyCycle(int duty);

void DACWrite(int slaveSelectPin, int address, int value)
{
	digitalWrite(slaveSelectPin, LOW);

	SPI.transfer(address);
	SPI.transfer(value);

	digitalWrite(slaveSelectPin, HIGH);
}

double getAngle(int channel);

void initializeSPI(void);
void initializeDac(void);
void initializePid(void);

void enablePid(void);
void disablePid(void);

#endif
