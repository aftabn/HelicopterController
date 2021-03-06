/*
Name: pidControl.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _PIDCONTROL_h
#define _PIDCONTROL_h

#include "arduino.h"
#include "globals.h"

#define ARDUINO_VSS						5.0

// For this project, CH0 -> Yaw w/ Encoder, CH1 -> Tilt w/ Potentiometer
#define ENCODER_CHANNEL					0
#define POTENTIOMETER_CHANNEL			1

#define YAW_CHANNEL						0
#define TILT_CHANNEL					1

// Since the yaw motor does not output the same amount of force in both directions (due to propeller orientation),
// there are 2 different PID profiles for yaw (one for each direction), and only one for tilt
#define NUM_DIRECTION_PROFILES			2
#define YAW_CW_DRECTION_PROFILE			0
#define YAW_CCW_DRECTION_PROFILE		1
#define TILT_DIRECTION_PROFILE			0

#define ENCODER_TOTAL_RANGE_DEGREES		360.0
#define ENCODER_PULSES_PER_REVOLUTION	400
#define ENCODER_DEGREES_PER_PULSE		ENCODER_TOTAL_RANGE_DEGREES / ENCODER_PULSES_PER_REVOLUTION

#define POT_TOTAL_RANGE_DEGREES			290.0
#define POT_DEGREES_PER_VOLT			POT_TOTAL_RANGE_DEGREES / ARDUINO_VSS
#define POT_MIN_IDLE_VOLTAGE			0.00
#define POT_MAX_IDLE_VOLTAGE			5.00
#define DEFAULT_POT_IDLE_VOLTAGE		2.05		// Voltage when the helicopter is balanced

#define ADC_REFERENCE_VOLTAGE			ARDUINO_VSS
#define ADC_RESOLUTION					1023
#define ADC_VOLTS_PER_BIT				ADC_REFERENCE_VOLTAGE / ADC_RESOLUTION
#define ADC_CHANNEL_MIN					0
#define ADC_CHANNEL_MAX					7
#define DEFAULT_NUM_ADC_SAMPLES			50

#define DAC_REFERENCE_VOLTAGE			ARDUINO_VSS
#define DAC_RESOLUTION					1023
#define DAC_VOLTS_PER_BIT				DAC_REFERENCE_VOLTAGE / DAC_RESOLUTION

// For the current driver:
#define MOTOR_VOLTAGE_RANGE				2.5		// Voltage range for each direction (i.e. 2.5V range for each direction)
#define MOTOR_IDLE_VOLTAGE				2.5		// Idle voltage
#define MOTOR_MIN_VOLTAGE				0.0		// Full CCW rotation
#define MOTOR_MAX_VOLTAGE				5.0		// Full CW rotation

#define MIN_FREQUENCY					0		// Corresponds to 0 RPM
#define MAX_FREQUENCY					10000	// Corresponds to 10000 RPM
#define DEFAULT_MAX_FREQUENCY			1000	// Corresponds to 5000 RPM

#define P_GAIN_MIN						0.0
#define P_GAIN_MAX						10.0

#define I_GAIN_MIN						0.0
#define I_GAIN_MAX						10.0

#define I_WINDUP_THRESH_MIN				0.0
#define I_WINDUP_THRESH_MAX				100.0

#define OUTPUT_RATE_LIMIT_MIN			1
#define OUTPUT_RATE_LIMIT_MAX			50

#define D_GAIN_MIN						0.0
#define D_GAIN_MAX						10.0

#define PID_OUTPUT_MIN					0
#define PID_OUTPUT_MAX					100

#define YAW_SET_POINT_MIN				-180.0
#define YAW_SET_POINT_MAX				180.0

#define TILT_SET_POINT_MIN				-45.0
#define TILT_SET_POINT_MAX				90.0

#define YAW_OUTPUT_MIN					-100
#define YAW_OUTPUT_MAX					100

#define TILT_OUTPUT_MIN					0
#define TILT_OUTPUT_MAX					100

#define TILT_OUTPUT_OFFSET_MIN			0
#define TILT_OUTPUT_OFFSET_MAX			50

#define PID_INTERVAL_MS_MIN				1		// Minimum timer 1 overflow is 1 ms
#define PID_INTERVAL_MS_MAX				260		// Maximum timer 1 overflow is ~260 ms

#define DEFAULT_DIRECTION				0		// Corresponds to Clockwise
#define DEFAULT_MOTOR_DRIVER_TYPE		0		// Corresponds to AnalogVoltage
#define DEFAULT_P_GAIN					P_GAIN_MIN
#define DEFAULT_I_GAIN					I_GAIN_MIN
#define DEFAULT_D_GAIN					D_GAIN_MIN
#define DEFAULT_SET_POINT				0.0
#define DEFAULT_I_WINDUP_THRESH			30.0
#define DEFAULT_OUTPUT_RATE_LIMIT		10
#define DEFAULT_PID_INTERVAL_MS			250
#define DEFAULT_TILT_OUTPUT_OFFSET		40

enum Direction { Clockwise, CounterClockwise };
enum MotorDriverType { AnalogVoltage, Frequency };

extern const int minMotorOutput[MAX_NUM_CHANNELS];
extern const int maxMotorOutput[MAX_NUM_CHANNELS];
extern const double minSetPoint[MAX_NUM_CHANNELS];
extern const double maxSetPoint[MAX_NUM_CHANNELS];

extern volatile int tiltOutputOffset;	// This is the minimum output needed for the motor to lift
extern volatile double potIdleVoltage;

extern volatile bool isPidCalculationNeeded;
extern volatile bool isPidEnabled;
extern volatile bool isVerboseMode;
extern volatile bool isSafetyOn;

extern volatile int pidLoopInterval;
extern volatile uint16_t currentFrequency;
extern volatile uint16_t maxFrequency;

extern volatile double pGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
extern volatile double iGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
extern volatile double dGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
extern volatile double iWindupThresholds[MAX_NUM_CHANNELS];
extern volatile int outputRateLimits[MAX_NUM_CHANNELS];
extern volatile double setPoints[MAX_NUM_CHANNELS];
extern volatile double currentVoltages[MAX_NUM_CHANNELS];
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
void initializePotentiometerTimer(void);

void disableMotors(void);

void enablePid(void);
void disablePid(void);
void resetPidValues(void);

void executePidCalculation(void);
void updatePidMotorOutputs(int channel, Direction *direction, int *percentageOutput);
void scaleIntegratedError(int channel);
void calculateNewOutput(int channel, int &newOutput);
void constrainOutput(int channel, int &newOutput, int &currentOutput);
void applyMotorOutputs(int channel, Direction direction, int percentageOutput);

double adjustOutputToVoltage(Direction direction, int percentageOutput);
uint16_t adjustOutputToFrequency(int percentageOutput);

double getSampledAdcVoltage(int adcChannel);
double getSampledAdcVoltage(int adcChannel, uint8_t numSamples);
double getAdcVoltage(int adcChannel);
int getAdcValue(int adcChannel);
double convertAdcValueToVoltage(int adcValue);

double convertPotentiometerVoltageToAngle(double voltage);
double getPotentiometerAngle(int channel);
void updatePotentiometerAngle(void);

void zeroEncoderAngle(void);

int convertVoltageToDacValue(double voltage);
void setDacVoltage(int channel, double voltage);

// For this project, ONLY ONE MOTOR can use frequency control as the current implementation only has enough
//	 hardware to control one motor.
void setFrequency(uint16_t frequency);
void disableFrequency(void);

#endif
