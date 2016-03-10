/*
Name: pidControl.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <SPI\SPI.h>
#include <digitalWriteFast.h>
#include <Streaming.h>
#include "util.h"
#include "pidControl.h"

const byte adcChannelLookup[] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
const int minMotorOutput[MAX_NUM_CHANNELS] = { YAW_OUTPUT_MIN, TILT_OUTPUT_MIN };
const int maxMotorOutput[MAX_NUM_CHANNELS] = { YAW_OUTPUT_MAX, TILT_OUTPUT_MAX };
const double minSetPoint[MAX_NUM_CHANNELS] = { YAW_SET_POINT_MIN, TILT_SET_POINT_MIN };
const double maxSetPoint[MAX_NUM_CHANNELS] = { YAW_SET_POINT_MAX, TILT_SET_POINT_MAX };

volatile bool isPidCalculationNeeded;
volatile bool isPidEnabled;
volatile bool isVerboseMode;
volatile bool isSafetyOn;

volatile int pidLoopInterval;
volatile uint16_t currentFrequency;

const signed int encoderLookup[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
byte encoderValues;

volatile double pGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
volatile double iGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
volatile double dGains[MAX_NUM_CHANNELS][NUM_DIRECTION_PROFILES];
volatile double iWindupThresholds[MAX_NUM_CHANNELS];
volatile int outputRateLimits[MAX_NUM_CHANNELS];
volatile double setPoints[MAX_NUM_CHANNELS];
volatile double currentVoltages[MAX_NUM_CHANNELS];
volatile double currentAngles[MAX_NUM_CHANNELS];
volatile double previousAngles[MAX_NUM_CHANNELS];
volatile int currentOutputs[MAX_NUM_CHANNELS];
volatile Direction directions[MAX_NUM_CHANNELS];
volatile MotorDriverType motorDriverTypes[MAX_NUM_CHANNELS];

double angleErrors[MAX_NUM_CHANNELS];
double integratedAngleErrors[MAX_NUM_CHANNELS];
double derivativeAnglesErrors[MAX_NUM_CHANNELS];

// This is the ISR that runs the PID algorithm
ISR(TIMER1_COMPA_vect)
{
	if (isPidEnabled)
	{
		isPidCalculationNeeded = true;
	}
}

// Taken and modified from: http://www.mkesc.co.uk/ise.pdf
static void quadratureDecoderISR(void)
{
	encoderValues <<= 2;
	encoderValues |= ((digitalReadFast(ENCODER_CHA_PIN) << 1) | digitalReadFast(ENCODER_CHB_PIN));
	currentAngles[ENCODER_CHANNEL] += encoderLookup[encoderValues & 0x0F] * ENCODER_DEGREES_PER_PULSE;
}

void initializeSpi(void)
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
		for (int profile = 0; profile < NUM_DIRECTION_PROFILES; profile++)
		{
			pGains[channel][profile] = DEFAULT_P_GAIN;
			iGains[channel][profile] = DEFAULT_I_GAIN;
			dGains[channel][profile] = DEFAULT_D_GAIN;
		}

		iWindupThresholds[channel] = DEFAULT_I_WINDUP_THRESH;
		outputRateLimits[channel] = DEFAULT_OUTPUT_RATE_LIMIT;
		setPoints[channel] = DEFAULT_SET_POINT;
		currentAngles[channel] = 0;
		directions[channel] = (Direction)DEFAULT_DIRECTION;
		motorDriverTypes[channel] = (MotorDriverType)DEFAULT_MOTOR_DRIVER_TYPE;
	}
}

void initializeFrequencyOutput(void)
{
	const int dummyChannel = 0; // Only one frequency output so channel doesn't matter

	pinModeFast(FREQUENCY_OUTPUT_PIN, OUTPUT);
	setFrequency(dummyChannel, MOTOR_MIN_FREQUENCY);
}

void initializeQuadratureDecoder(void)
{
	pinModeFast(ENCODER_CHA_PIN, INPUT);
	pinModeFast(ENCODER_CHB_PIN, INPUT);
	encoderValues = (digitalReadFast(ENCODER_CHA_PIN) << 1) | digitalReadFast(ENCODER_CHB_PIN);

	// Sets ISR for external interrupt on pin 2
	attachInterrupt(0, quadratureDecoderISR, RISING);
}

void initializeAdc(void)
{
	pinModeFast(ADC_CHIP_SELECT_PIN, OUTPUT);
	digitalWriteFast(ADC_CHIP_SELECT_PIN, LOW);
	digitalWriteFast(ADC_CHIP_SELECT_PIN, HIGH);
}

void initializeDac(void)
{
	pinModeFast(DAC_CHIP_SELECT_PIN, OUTPUT);
	digitalWriteFast(DAC_CHIP_SELECT_PIN, LOW);
	digitalWriteFast(DAC_CHIP_SELECT_PIN, HIGH);

	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		setDacVoltage(channel, MOTOR_IDLE_VOLTAGE);
	}
}

// Initializes the PID Timer (Timer 1) with the appropriate interval in milliseconds
void initializePidTimer(int numMilliseconds)
{
	noInterrupts();							// disable all interrupts
	TCCR1A = 0;								// set entire TCCR1A register to 0
	TCCR1B = 0;								// same for TCCR1B
	TCNT1 = 0;								// initialize counter value to 0
	OCR1A = 250 * numMilliseconds - 1;		// compare match register (16MHz/64/1000 = 250) * numMilliseconds - 1
	TCCR1B |= (1 << WGM12);					// CTC mode
	TCCR1B |= (1 << CS11) | (1 << CS10);	// 64 prescaler
	TIMSK1 |= (1 << OCIE1A);				// enable timer overflow interrupt
	interrupts();							// enable all interrupts
}

void executePidCalculation()
{
	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		int percentageOutput;
		Direction direction;

		// Updates the direction and the output
		updatePidMotorOutputs(channel, &direction, &percentageOutput);

		// Makes the necessary hardware output changes based on driver type
		applyMotorOutputs(channel, direction, percentageOutput);

		if (isVerboseMode)
		{
			// Format is: "[CH%d] SP: %s deg, Output: %d %%, Angle: %s deg"
			Serial << F("[CH") << channel << F("] SP: ") << setPoints[channel] << F("deg, Output: ") << currentOutputs[channel] << F("%, Angle: ") << currentAngles[channel] << F(" degs") << NEWLINE;
		}
	}
}

void updatePidMotorOutputs(int channel, Direction *direction, int *percentageOutput)
{
	// Get output as a signed value from -100 to 100 based on direction for easier calculations
	int currentOutput = currentOutputs[channel] * (directions[channel] == Clockwise ? 1 : -1);

	// P term
	angleErrors[channel] = setPoints[channel] - currentAngles[channel];

	// TODO: Put this in a function
	// This scales the integrated error based on the igain of the direction its moving in
	if (channel == YAW_CHANNEL)
	{
		if (previousAngles[channel] > setPoints[channel])
		{
			if (currentAngles[YAW_CHANNEL] < setPoints[YAW_CHANNEL])
			{
				integratedAngleErrors[YAW_CHANNEL] *= iGains[YAW_CHANNEL][YAW_CCW_DRECTION_PROFILE] / iGains[YAW_CHANNEL][YAW_CW_DRECTION_PROFILE];
			}
		}
		else
		{
			if (currentAngles[YAW_CHANNEL] > setPoints[YAW_CHANNEL])
			{
				integratedAngleErrors[YAW_CHANNEL] *= iGains[YAW_CHANNEL][YAW_CW_DRECTION_PROFILE] / iGains[YAW_CHANNEL][YAW_CCW_DRECTION_PROFILE];
			}
		}
	}

	// I term
	if (abs(angleErrors[channel]) < iWindupThresholds[channel])
	{
		integratedAngleErrors[channel] = integratedAngleErrors[channel] + angleErrors[channel] * pidLoopInterval / 1000.0;
	}

	// D term
	derivativeAnglesErrors[channel] = (currentAngles[channel] - previousAngles[channel]) / (pidLoopInterval / 1000.0);

	int newOutput = 0;

	// TODO: Refactor all this
	// Update previous angle for next calculation
	previousAngles[channel] = currentAngles[channel];

	if (channel == TILT_CHANNEL)
	{
		if (setPoints[channel] > -17)
		{
			newOutput += 40;
		}
	}

	// Get new signed output from PID algorithm
	if (channel == YAW_CHANNEL)
	{
		if (angleErrors[channel] > 0)
		{
			newOutput += (int)(pGains[channel][YAW_CW_DRECTION_PROFILE] * angleErrors[channel] + iGains[channel][YAW_CW_DRECTION_PROFILE] * integratedAngleErrors[channel] - dGains[channel][YAW_CW_DRECTION_PROFILE] * derivativeAnglesErrors[channel]);
		}
		else
		{
			newOutput += (int)(pGains[channel][YAW_CCW_DRECTION_PROFILE] * angleErrors[channel] + iGains[channel][YAW_CCW_DRECTION_PROFILE] * integratedAngleErrors[channel] - dGains[channel][YAW_CCW_DRECTION_PROFILE] * derivativeAnglesErrors[channel]);
		}
	}
	else
	{
		newOutput += (int)(pGains[channel][TILT_DIRECTION_PROFILE] * angleErrors[channel] + iGains[channel][TILT_DIRECTION_PROFILE] * integratedAngleErrors[channel] - dGains[channel][TILT_DIRECTION_PROFILE] * derivativeAnglesErrors[channel]);
	}

	// Limit the amount the output can change by
	if (abs(newOutput - currentOutput) > outputRateLimits[channel])
	{
		if (newOutput > currentOutput)
		{
			newOutput = currentOutput + outputRateLimits[channel];
		}
		else
		{
			newOutput = currentOutput - outputRateLimits[channel];
		}
	}

	// Constrain the output between the upper and lower bounds
	newOutput = max(min(newOutput, maxMotorOutput[channel]), minMotorOutput[channel]);

	// Apply the changes and switch output back to between 0 and 100 %
	*percentageOutput = abs(newOutput);
	*direction = newOutput > 0 ? Clockwise : CounterClockwise;
}

void applyMotorOutputs(int channel, Direction direction, int percentageOutput)
{
	if (motorDriverTypes[channel] == AnalogVoltage)
	{
		double voltage = adjustOutputToVoltage(direction, percentageOutput);
		setDacVoltage(channel, voltage);
	}
	else if (motorDriverTypes[channel] == Frequency)
	{
		if (percentageOutput != 0)
		{
			int frequency = adjustOutputToFrequency(percentageOutput);
			setFrequency(channel, frequency);
		}
		else
		{
			disableFrequency();
		}
	}

	currentOutputs[channel] = percentageOutput;
	directions[channel] = direction;
}

double adjustOutputToVoltage(Direction direction, int percentageOutput)
{
	double voltage = MOTOR_IDLE_VOLTAGE;

	voltage += direction == Clockwise ? MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0 : -1 * MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0;

	return voltage;
}

uint16_t adjustOutputToFrequency(int percentageOutput)
{
	uint16_t frequency = (MOTOR_MAX_FREQUENCY - MOTOR_MIN_FREQUENCY) * percentageOutput / 100.0;
	return frequency;
}

int convertVoltageToDacValue(double voltage)
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

	noInterrupts();
	digitalWriteFast(DAC_CHIP_SELECT_PIN, LOW);
	SPI.transfer(primaryByte);
	SPI.transfer(secondaryByte);
	digitalWriteFast(DAC_CHIP_SELECT_PIN, HIGH);
	interrupts();

	currentVoltages[channel] = voltage;
}

void setFrequency(int channel, uint16_t frequency)
{
	tone(FREQUENCY_OUTPUT_PIN, frequency);
	digitalWriteFast(FREQUENCY_DIRECTION_PIN, directions[channel] == Clockwise ? HIGH : LOW);

	currentFrequency = frequency;
}

double getSampledAdcVoltage(int channel)
{
	return getSampledAdcVoltage(channel, DEFAULT_NUM_ADC_SAMPLES);
}

double getSampledAdcVoltage(int channel, uint8_t numSamples)
{
	double adcVoltage = 0;

	for (int i = 0; i < numSamples; i++)
	{
		adcVoltage += getAdcVoltage(channel);
	}

	adcVoltage /= numSamples;

	return adcVoltage;
}

// Unlike the other functions, the MCP3008 ADC has 8 channels, which means
// the input parameter can be between 0 and 7
// Source for code: https://rheingoldheavy.com/mcp3008-tutorial-02-sampling-dc-voltage/
int getAdcValue(int channel)
{
	SPISettings MCP3008(2000000, MSBFIRST, SPI_MODE0);

	byte readAddress = adcChannelLookup[channel];
	byte dataMSB = 0;
	byte dataLSB = 0;
	byte JUNK = 0x00;

	noInterrupts();
	SPI.beginTransaction(MCP3008);
	digitalWriteFast(ADC_CHIP_SELECT_PIN, LOW);
	SPI.transfer(0x01);									// Start Bit
	dataMSB = SPI.transfer(readAddress << 4) & 0x03;	// Send readAddress and receive MSB data, masked to two bits
	dataLSB = SPI.transfer(JUNK);						// Push junk data and get LSB byte return
	digitalWriteFast(ADC_CHIP_SELECT_PIN, HIGH);
	SPI.endTransaction();
	interrupts();

	int adcValue = dataMSB << 8 | dataLSB;				// Storing in variable first for debugging

	return adcValue;
}

double getAdcVoltage(int channel)
{
	int adcValue = getAdcValue(channel);
	double voltage = convertAdcValueToVoltage(adcValue);
	return voltage;
}

double convertAdcValueToVoltage(int adcValue)
{
	double voltage = (double)adcValue / ADC_RESOLUTION * ADC_REFERENCE_VOLTAGE;
	return voltage;
}

double convertPotentiometerVoltageToAngle(double voltage)
{
	double angle = (voltage - POT_IDLE_VOLTAGE) * POT_DEGREES_PER_VOLT;
	return angle;
}

double getPotentiometerAngle(int channel)
{
	double voltage = getSampledAdcVoltage(channel);
	double angle = convertPotentiometerVoltageToAngle(voltage);

	return angle;
}

void updatePotentiometerAngle(void)
{
	double angle = getPotentiometerAngle(POTENTIOMETER_CHANNEL);
	currentAngles[POTENTIOMETER_CHANNEL] = angle;
}

void zeroEncoderAngle()
{
	currentAngles[YAW_CHANNEL] = 0;
}

void disableMotors()
{
	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		applyMotorOutputs(channel, directions[channel], 0);
	}
}

void disableFrequency()
{
	noTone(FREQUENCY_OUTPUT_PIN);
	currentFrequency = 0;
}

void enablePid()
{
	resetPidValues();
	initializePidTimer(pidLoopInterval);
	isPidEnabled = true;
}

void disablePid()
{
	isPidEnabled = false;
	disableMotors();
}

void resetPidValues()
{
	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		previousAngles[channel] = 0;
		angleErrors[channel] = 0;
		integratedAngleErrors[channel] = 0;
		derivativeAnglesErrors[channel] = 0;
	}
}