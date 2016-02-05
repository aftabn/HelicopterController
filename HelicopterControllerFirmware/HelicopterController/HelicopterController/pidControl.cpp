/*
Name: pidControl.cpp
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#include <SPI\SPI.h>
#include "util.h"
#include "pidControl.h"

const byte adc_read_channels[8] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

volatile bool isPidEnabled;
volatile bool isDebugMode;
volatile bool isSafetyOn;

volatile int pidLoopInterval;

volatile bool previousEncoderA, previousEncoderB;

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
	char tmpstr[70];

	if (isPidEnabled)
	{
		for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
		{
			int percentageOutput;
			Direction direction;

			// Updates the direction and the output
			updatePidMotorOutputs(channel, &direction, &percentageOutput);

			// Makes the necessary hardware output changes based on driver type
			if (motorDriverTypes[channel] == AnalogVoltage)
			{
				int voltage = adjustOutputToVoltage(direction, percentageOutput);
				setDacVoltage(channel, voltage);
			}
			else if (motorDriverTypes[channel] == Frequency)
			{
				int frequency = adjustOutputToFrequency(percentageOutput);
				setFrequency(channel, frequency);
			}

			if (isDebugMode)
			{
				char setpoint[8];
				dtostrf(setPoints[channel], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, setpoint);
				char angle[8];
				dtostrf(currentAngles[channel], MIN_NUMBER_FLOAT_CHARS, DEFAULT_NUM_DECIMALS, angle);
				sprintf(tmpstr, "[CH%d] SP: %s deg, Output: %d %%, Angle: %s deg",
					channel, setpoint, currentOutputs[channel], angle);

				Serial.println(tmpstr);
			}
		}
	}
}

static void quadratureDecoderISR(void)
{
	bool currentEncoderA, currentEncoderB;
	bool isForward, isError;

	currentEncoderA = (bool)digitalRead(ENCODER_CHA_PIN);
	currentEncoderB = (bool)digitalRead(ENCODER_CHB_PIN);

	if (!previousEncoderA && !previousEncoderB) {
		isForward = currentEncoderA;
	}
	else if (previousEncoderA && !previousEncoderB) {
		isForward = currentEncoderB;
	}
	else if (previousEncoderA && previousEncoderB) {
		isForward = !currentEncoderA;
	}
	else if (!previousEncoderA && previousEncoderB) {
		isForward = !currentEncoderB;
	}
	else {
		isError = true;
	}

	if (!isError)
	{
		currentAngles[ENCODER_CHANNEL] += isForward ? 1 : -1;
	}

	previousEncoderA = currentEncoderA;
	previousEncoderB = currentEncoderB;
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
	isSafetyOn = false; // TODO: Change this later

	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		pGains[channel] = DEFAULT_P_GAIN;
		iGains[channel] = DEFAULT_I_GAIN;
		dGains[channel] = DEFAULT_D_GAIN;
		setPoints[channel] = DEFAULT_SET_POINT;
		currentAngles[channel] = 0;
		directions[channel] = (Direction)DEFAULT_DIRECTION;
		motorDriverTypes[channel] = (MotorDriverType)DEFAULT_MOTOR_DRIVER_TYPE;
	}
}

void initializeFrequencyOutput(void)
{
	const int dummyChannel = 0; // Only one frequency output so channel doesn't matter during initialization

	pinMode(FREQUENCY_OUTPUT_PIN, OUTPUT);
	setFrequency(dummyChannel, MOTOR_MIN_FREQUENCY);
}

void initializeQuadratureDecoder(void)
{
	pinMode(ENCODER_CHA_PIN, INPUT);
	pinMode(ENCODER_CHB_PIN, INPUT);
	previousEncoderA = (bool)digitalRead(ENCODER_CHA_PIN);
	previousEncoderB = (bool)digitalRead(ENCODER_CHB_PIN);

	// Sets ISR for external interrupt on pin 2
	attachInterrupt(0, quadratureDecoderISR, RISING);
}

void initializeAdc(void)
{
	pinMode(ADC_CHIP_SELECT_PIN, OUTPUT);
	digitalWrite(ADC_CHIP_SELECT_PIN, LOW);
	digitalWrite(ADC_CHIP_SELECT_PIN, HIGH);
}

void initializeDac(void)
{
	pinMode(DAC_CHIP_SELECT_PIN, OUTPUT);
	digitalWrite(DAC_CHIP_SELECT_PIN, LOW);
	digitalWrite(DAC_CHIP_SELECT_PIN, HIGH);

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

int adjustOutputToVoltage(Direction direction, int percentageOutput)
{
	int voltage = MOTOR_IDLE_VOLTAGE;

	voltage += direction == Clockwise ? MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0 : -1 * MOTOR_VOLTAGE_RANGE * percentageOutput / 100.0;

	return voltage;
}

int adjustOutputToFrequency(int percentageOutput)
{
	int frequency = MOTOR_FREQUENCY_RANGE * percentageOutput / 100.0;
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

	// Not sure if disabling interrupts is necessary as yet

	//noInterrupts();
	digitalWrite(DAC_CHIP_SELECT_PIN, LOW);
	SPI.transfer(primaryByte);
	SPI.transfer(secondaryByte);
	digitalWrite(DAC_CHIP_SELECT_PIN, HIGH);
	//interrupts();
}

void setFrequency(int channel, int frequency)
{
	tone(FREQUENCY_OUTPUT_PIN, frequency);
	digitalWrite(FREQUENCY_DIRECTION_PIN, directions[channel] == Clockwise ? HIGH : LOW);
}

// Unlike the other functions, the MCP3008 ADC has 8 channels, which means
// the input parameter can be between 0 and 7
// Source for code: https://rheingoldheavy.com/mcp3008-tutorial-02-sampling-dc-voltage/
int getAdcValue(int channel)
{
	SPISettings MCP3008(2000000, MSBFIRST, SPI_MODE0);

	byte readAddress = adc_read_channels[channel];
	byte dataMSB = 0;
	byte dataLSB = 0;
	byte JUNK = 0x00;

	SPI.beginTransaction(MCP3008);
	digitalWrite(ADC_CHIP_SELECT_PIN, LOW);
	SPI.transfer(0x01);									// Start Bit
	dataMSB = SPI.transfer(readAddress << 4) & 0x03;	// Send readAddress and receive MSB data, masked to two bits
	dataLSB = SPI.transfer(JUNK);						// Push junk data and get LSB byte return
	digitalWrite(ADC_CHIP_SELECT_PIN, HIGH);
	SPI.endTransaction();

	int adcValue = dataMSB << 8 | dataLSB;				// Storing in variable first for debugging

	return adcValue;
}

double convertAdcValueToVoltage(int adcValue)
{
	double voltage = (double)adcValue / ADC_RESOLUTION * ADC_REFERENCE_VOLTAGE;
	return voltage;
}

double getAdcVoltage(int channel)
{
	int adcValue = getAdcValue(channel);
	double voltage = convertAdcValueToVoltage(adcValue);
	return voltage;
}

void disableMotors()
{
	for (int channel = 0; channel < MAX_NUM_CHANNELS; channel++)
	{
		if (motorDriverTypes[channel] == AnalogVoltage)
		{
			setDacVoltage(channel, MOTOR_IDLE_VOLTAGE);
		}
		else if (motorDriverTypes[channel] == Frequency)
		{
			setFrequency(channel, MOTOR_MIN_FREQUENCY);
		}
	}
}

void enablePid()
{
	initializePidTimer(pidLoopInterval);
	isPidEnabled = true;
}

void disablePid()
{
	isPidEnabled = false;
	disableMotors();
}

void enableDebug()
{
	isDebugMode = true;
}

void disableDebug()
{
	isDebugMode = false;
}