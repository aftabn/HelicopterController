/*
Name: pidControl.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <SPI\SPI.h>
#include "util.h"
#include "pidControl.h"

volatile bool isPidEnabled;
volatile bool isDebugMode;
volatile bool isSafetyOn;

volatile int pidLoopInterval;

volatile double pGains[MAX_NUM_CHANNELS];
volatile double iGains[MAX_NUM_CHANNELS];
volatile double dGains[MAX_NUM_CHANNELS];
volatile double setPoints[MAX_NUM_CHANNELS];
volatile double currentAngles[MAX_NUM_CHANNELS];
volatile double previousAngles[MAX_NUM_CHANNELS];
volatile int currentOutputs[MAX_NUM_CHANNELS];
volatile Direction directions[MAX_NUM_CHANNELS];
volatile MotorDriverType motorDriverTypes[MAX_NUM_CHANNELS];

double angleErrors[MAX_NUM_CHANNELS];
double integratedAngleErrors[MAX_NUM_CHANNELS];
double derivativeAnglesErrors[MAX_NUM_CHANNELS];

// This is the ISR that runs the PID algorithm
ISR(TIMER1_OVF_vect)
{
	char tmpstr[100];

	if (isPidEnabled)
	{
		for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
		{
			int percentageOutput;
			Direction direction;

			updatePidMotorOutputs(channel, &direction, &percentageOutput);

			if (motorDriverTypes[channel] == AnalogVoltage)
			{
				int voltage = adjustOutputToVoltage(direction, percentageOutput);
				setDacVoltage(channel, voltage);
			}
			else if (motorDriverTypes[channel] == Frequency)
			{
				int frequency = adjustOutputToFrequency(direction, percentageOutput);
				setFrequency(frequency);
			}

			if (isDebugMode)
			{
				sprintf(tmpstr, "[CH%d] SP: %.1f deg, Output: %d %%, Angle: %03.1f deg",
					channel, setPoints[channel], currentOutputs[channel], currentAngles[channel]);

				Serial.println(tmpstr);
			}
		}
	}
}

void initializeSPI(void)
{
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

void initializePid(void)
{
	pidLoopInterval = DEFAULT_PID_INTERVAL_MS;
	isSafetyOn = true;

	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		pGains[channel] = DEFAULT_P_GAIN;
		iGains[channel] = DEFAULT_I_GAIN;
		dGains[channel] = DEFAULT_D_GAIN;
		setPoints[channel] = DEFAULT_SET_POINT;
		directions[channel] = (Direction)DEFAULT_DIRECTION;
		motorDriverTypes[channel] = (MotorDriverType)DEFAULT_MOTOR_DRIVER_TYPE;
	}
}

void initializeFrequencyOutput(void)
{
	pinMode(FREQUENCY_OUTPUT_PIN, OUTPUT);
	setFrequency(MOTOR_IDLE_FREQUENCY);
}

void initializeDac(void)
{
	pinMode(DAC_SLAVE_SELECT_PIN, OUTPUT);
	digitalWrite(DAC_SLAVE_SELECT_PIN, HIGH);

	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		setDacVoltage(channel, MOTOR_IDLE_VOLTAGE);
	}
}

// Reinitializes the PID Timer (Timer 1) with the appropriate interval in milliseconds
void initializePidTimer(int numMilliseconds)
{
	noInterrupts();							// disable all interrupts
	TCCR1A = 0;								// set entire TCCR1A register to 0
	TCCR1B = 0;								// same for TCCR1B
	TCNT1 = 0;								// initialize counter value to 0
	OCR1A = 250 * numMilliseconds - 1;		// compare match register (16MHz/64/1000 = 250) * numMilliseconds - 1
	TCCR1B |= (1 << CS11) | (1 << CS10);	// 64 prescaler
	TIMSK1 |= (1 << TOIE1);					// enable timer overflow interrupt
	interrupts();							// enable all interrupts
}

void updatePidMotorOutputs(int channel, Direction* direction, int* percentageOutput)
{
	double currentAngle = currentAngles[channel];

	angleErrors[channel] = setPoints[channel] - currentAngles[channel];
	integratedAngleErrors[channel] = (abs(angleErrors[channel] < I_GAIN_THRESHHOLD_ERROR) ? integratedAngleErrors[channel] + angleErrors[channel] * pidLoopInterval : 0);
	derivativeAnglesErrors[channel] = (currentAngles[channel] - previousAngles[channel]) / pidLoopInterval;

	previousAngles[channel] = currentAngle;

	int output = (int)(pGains[channel] * angleErrors[channel] + iGains[channel] * integratedAngleErrors[channel] + dGains[channel] * derivativeAnglesErrors[channel]);
	*percentageOutput = max(min(output, 100), 0);
	*direction = angleErrors[channel] > 0 ? Clockwise : CounterClockwise;
}

static int adjustOutputToVoltage(Direction direction, int percentageOutput)
{
	int voltage = MOTOR_IDLE_VOLTAGE;

	voltage += direction == Clockwise ? MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0 : -1 * MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0;

	return voltage;
}

static int adjustOutputToFrequency(Direction direction, int percentageOutput)
{
	int frequency = MOTOR_IDLE_FREQUENCY;

	frequency += direction == Clockwise ? MOTOR_MAX_FREQUENCY * percentageOutput / 100.0 : -1 * MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0;

	return frequency;
}

static int convertVoltageToDacValue(double voltage)
{
	int value = (int)(voltage / DAC_REFERENCE_VOLTAGE * DAC_RESOLUTION);
	return value;
}

// TODO: Refactor this once requirements and sensors have been finalized
// Refer to : https://github.com/wigman27/Tutorial-Using-Arduino-SPI for more info
void setDacVoltage(int channel, double voltage)
{
	int value = convertVoltageToDacValue(voltage);

	// Bit Significance: 1st = channel, 2nd = bypasses input buffer, 3rd = output gain x1, 4th = active low SHDN, LSBs don't matter
	int defaultDacRegister = 0B00110000;					//
	int secondByteMask = 0b0000000011111111;				// For selecting last 8 bits
	byte primaryByte = (value >> 6) | defaultDacRegister;	// Isolates the 2 MSB for ORing
	byte secondaryByte = (value << 2) & secondByteMask;		// Isolates the 8 LSB

	// Sets the MSB in the primaryByte to determine the DAC to be set, DAC A=0, DAC B=1
	switch (channel) {
	case 0:
		primaryByte &= ~(1 << 7);
		break;
	case 1:
		primaryByte |= (1 << 7);
	}

	// Not sure if disabling interrupts is necessary as yet

	//noInterrupts();
	digitalWrite(DAC_SLAVE_SELECT_PIN, LOW);
	SPI.transfer(primaryByte);
	SPI.transfer(secondaryByte);
	digitalWrite(DAC_SLAVE_SELECT_PIN, HIGH);
	//interrupts();
}

void setFrequency(int frequency)
{
	tone(FREQUENCY_OUTPUT_PIN, frequency);
}

void enablePid(void)
{
	initializePidTimer(pidLoopInterval);
	isPidEnabled = true;
}

void disablePid(void)
{
	isPidEnabled = false;
}

void enableDebug(void)
{
	isDebugMode = true;
}

void disableDebug(void)
{
	isDebugMode = false;
}