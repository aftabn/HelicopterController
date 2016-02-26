/*
Name: dac.cpp
Created: 2/25/2016
Author:	Aftab
*/

#include "dac.h"

const double Dac::DBL_ReferenceVoltage = Utility::DBL_ArduinoVss;
const double Dac::DBL_VoltsPerBit = DBL_ReferenceVoltage / Dac::INT_Resolution;

Dac::Dac(double defaultVoltage) : DBL_DefaultVoltage(defaultVoltage)
{
}

void Dac::initialize()
{
	pinModeFast(Utility::PIN_DacChipSelect, OUTPUT);
	digitalWriteFast(Utility::PIN_DacChipSelect, LOW);
	digitalWriteFast(Utility::PIN_DacChipSelect, HIGH);

	for (int channel = 0; channel < Utility::INT_MaxNumChannels; channel++)
	{
		setVoltage(channel, DBL_DefaultVoltage);
	}
}

int Dac::convertVoltageToDacValue(double voltage)
{
	int value = (int)(voltage / DBL_ReferenceVoltage * INT_Resolution);
	return value;
}

// Refer to : https://github.com/wigman27/Tutorial-Using-Arduino-SPI for more info
void Dac::setVoltage(int channel, double voltage)
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
	digitalWriteFast(Utility::PIN_DacChipSelect, LOW);
	SPI.transfer(primaryByte);
	SPI.transfer(secondaryByte);
	digitalWriteFast(Utility::PIN_DacChipSelect, HIGH);
	interrupts();

	currentVoltages[channel] = voltage;
}