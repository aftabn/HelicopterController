/*
Name: pidControl.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _PIDCONTROL_h
#define _PIDCONTROL_h

#include "arduino.h"
#include "globals.h"

#define DAC_SLAVE_SELECT_PIN		10
#define DAC_VOLTS_PER_BIT			0.0048876
#define DAC_RESOLUTION				1023
#define DAC_REFERENCE_VOLTAGE		5.0

// TODO: Change this
#define FREQUENCY_OUTPUT_PIN		7

// For the current driver:
#define MOTOR_VOLTAGE_RANGE			2.5		// Voltage range for each direction (i.e. 2.5V range for each direction)
#define MOTOR_IDLE_VOLTAGE			2.5		// Idle voltage
#define MOTOR_MAX_VOLTAGE			5.0		// Full CW rotation
#define MOTOR_MIN_VOLTAGE			0.0		// Full CCW rotation

#define MOTOR_FREQUENCY_RANGE		15000	// Frequency range for each direction (i.e. 5000Hz range for each direction)
#define MOTOR_IDLE_FREQUENCY		15000	// Idle frequency
#define MOTOR_MAX_FREQUENCY			30000	// Full CW rotation
#define MOTOR_MIN_FREQUENCY			0	// Full CCW rotation

#define P_GAIN_MIN					0.01
#define P_GAIN_MAX					10.0

#define I_GAIN_MIN					0.0
#define I_GAIN_MAX					20.0
#define I_GAIN_THRESHHOLD_ERROR		10

#define D_GAIN_MIN					0.0
#define D_GAIN_MAX					10.0

#define PID_OUTPUT_MIN				0
#define PID_OUTPUT_MAX				100

#define SET_POINT_MIN				0.0
#define SET_POINT_MAX				360.0

#define PID_INTERVAL_MS_MIN			1		// Minimum timer 1 overflow is 1 ms
#define PID_INTERVAL_MS_MAX			260		// Maximum timer 1 overflow is ~260 ms

#define DEFAULT_DIRECTION			0		// Corresponds to Clockwise
#define DEFAULT_MOTOR_DRIVER_TYPE	0		// Corresponds to AnalogVoltage
#define DEFAULT_P_GAIN				P_GAIN_MIN
#define DEFAULT_I_GAIN				I_GAIN_MIN
#define DEFAULT_D_GAIN				D_GAIN_MIN
#define DEFAULT_SET_POINT			SET_POINT_MIN
#define DEFAULT_PID_INTERVAL_MS		PID_INTERVAL_MS_MIN

enum Direction { Clockwise, CounterClockwise };
enum MotorDriverType { AnalogVoltage, Frequency };

extern volatile bool isPidEnabled;
extern volatile bool isDebugMode;
extern volatile bool isSafetyOn;

extern volatile int pidLoopInterval;

extern volatile double pGains[MAX_NUM_CHANNELS];
extern volatile double iGains[MAX_NUM_CHANNELS];
extern volatile double dGains[MAX_NUM_CHANNELS];
extern volatile double setPoints[MAX_NUM_CHANNELS];
extern volatile double currentAngles[MAX_NUM_CHANNELS];
extern volatile double previousAngles[MAX_NUM_CHANNELS];
extern volatile int currentOutputs[MAX_NUM_CHANNELS];
extern volatile Direction directions[MAX_NUM_CHANNELS];
extern volatile MotorDriverType motorDriverTypes[MAX_NUM_CHANNELS];

void initializeSPI(void);
void initializePid(void);
void initializePidTimer(void);
void initializeFrequencyOutput(void);
void initializeDac(void);

void enablePid(void);
void disablePid(void);

void updatePidMotorOutputs(int channel, Direction* direction, int* percentageOutput);
static int adjustOutputToVoltage(Direction direction, int percentageOutput);
static int adjustOutputToFrequency(Direction direction, int percentageOutput);

static int convertVoltageToDacValue(double voltage);
void setDacVoltage(int channel, double voltage);

// For now there is no channel because there will only be one output for frequency control
void setFrequency(int frequency);

#endif
