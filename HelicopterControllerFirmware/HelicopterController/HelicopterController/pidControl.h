/*
Name: pidControl.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _PIDCONTROL_h
#define _PIDCONTROL_h

#include "arduino.h"
#include "globals.h"

// For this project, channel 0 is measured using an encoder and channel 1 with a pot
#define ENCODER_CHANNEL				0
#define POTENTIOMETER_CHANNEL		1

#define ADC_VOLTS_PER_BIT			0.0048876
#define ADC_RESOLUTION				1023
#define ADC_REFERENCE_VOLTAGE		5.0
#define ADC_CHANNEL_MIN				0
#define ADC_CHANNEL_MAX				7

#define DAC_VOLTS_PER_BIT			0.0048876
#define DAC_RESOLUTION				1023
#define DAC_REFERENCE_VOLTAGE		5.0

// For the current driver:
#define MOTOR_VOLTAGE_RANGE			2.5		// Voltage range for each direction (i.e. 2.5V range for each direction)
#define MOTOR_IDLE_VOLTAGE			2.5		// Idle voltage
#define MOTOR_MAX_VOLTAGE			5.0		// Full CW rotation
#define MOTOR_MIN_VOLTAGE			0.0		// Full CCW rotation

#define MOTOR_MAX_FREQUENCY			30000	// Corresponds to 30000 RPM
#define MOTOR_MIN_FREQUENCY			0		// Corresponds to 0 RPM
#define MOTOR_FREQUENCY_RANGE		30000	// Frequency range for each direction (This is the full
											//		range since direction is controlled by the direction pin)
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
#define DEFAULT_PID_INTERVAL_MS		10

enum Direction { Clockwise, CounterClockwise };
enum MotorDriverType { AnalogVoltage, Frequency };

extern const byte adc_read_channels[8];

extern volatile bool isPidEnabled;
extern volatile bool isDebugMode;
extern volatile bool isSafetyOn;

extern volatile int pidLoopInterval;

extern volatile bool previousEncoderA, previousEncoderB;
extern volatile bool currentEncoderA, currentEncoderB;

extern volatile double pGains[MAX_NUM_CHANNELS];
extern volatile double iGains[MAX_NUM_CHANNELS];
extern volatile double dGains[MAX_NUM_CHANNELS];
extern volatile double setPoints[MAX_NUM_CHANNELS];
extern volatile double currentAngles[MAX_NUM_CHANNELS];
extern volatile double previousAngles[MAX_NUM_CHANNELS];
extern volatile int currentOutputs[MAX_NUM_CHANNELS];
extern volatile Direction directions[MAX_NUM_CHANNELS];
extern volatile MotorDriverType motorDriverTypes[MAX_NUM_CHANNELS];

void initializeSpi(void);
void initializeAdc(void);
void initializeDac(void);
void initializeQuadratureDecoder(void);
void initializeFrequencyOutput(void);
void initializePid(void);
void initializePidTimer(void);

void enablePid(void);
void disablePid(void);

void updatePidMotorOutputs(int channel, Direction* direction, int* percentageOutput);
int adjustOutputToVoltage(Direction direction, int percentageOutput);
int adjustOutputToFrequency(int percentageOutput);

double getAdcVoltage(int channel);
int getAdcValue(int channel);
double convertAdcValueToVoltage(int adcValue);

int convertVoltageToDacValue(double voltage);
void setDacVoltage(int channel, double voltage);

// For this project, ONLY ONE MOTOR can use frequency control as the current implementation only has enough
//	 hardware to control one motor.
// Channel is only used to determine which motor is the frequency motor
void setFrequency(int channel, int frequency);

#endif
