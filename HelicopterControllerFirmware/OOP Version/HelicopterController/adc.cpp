//
//
//

#include <SPI\SPI.h>
#include "adc.h"

const double Adc::DBL_ReferenceVoltage = Utility::DBL_ArduinoVss;
const double Adc::DBL_VoltsPerBit = DBL_ReferenceVoltage / Adc::INT_Resolution;

const byte Adc::adcChannelLookup[] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

void Adc::initialize()
{
	pinModeFast(Utility::PIN_AdcChipSelect, OUTPUT);
	digitalWriteFast(Utility::PIN_AdcChipSelect, LOW);
	digitalWriteFast(Utility::PIN_AdcChipSelect, HIGH);
}

double Adc::getSampledAdcVoltage(int channel)
{
	return getSampledAdcVoltage(channel, INT_DefaultNumSamples);
}

double Adc::getSampledAdcVoltage(int channel, uint8_t numSamples)
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
int Adc::getAdcValue(int channel)
{
	SPISettings MCP3008(2000000, MSBFIRST, SPI_MODE0);

	byte readAddress = adcChannelLookup[channel];
	byte dataMSB = 0;
	byte dataLSB = 0;
	byte JUNK = 0x00;

	noInterrupts();
	SPI.beginTransaction(MCP3008);
	digitalWriteFast(Utility::PIN_AdcChipSelect, LOW);
	SPI.transfer(0x01);									// Start Bit
	dataMSB = SPI.transfer(readAddress << 4) & 0x03;	// Send readAddress and receive MSB data, masked to two bits
	dataLSB = SPI.transfer(JUNK);						// Push junk data and get LSB byte return
	digitalWriteFast(Utility::PIN_AdcChipSelect, HIGH);
	SPI.endTransaction();
	interrupts();

	int adcValue = dataMSB << 8 | dataLSB;				// Storing in variable first for debugging

	return adcValue;
}

double Adc::getAdcVoltage(int channel)
{
	int adcValue = getAdcValue(channel);
	double voltage = convertAdcValueToVoltage(adcValue);
	return voltage;
}

double Adc::convertAdcValueToVoltage(int adcValue)
{
	double voltage = (double)adcValue / INT_Resolution * DBL_ReferenceVoltage;
	return voltage;
}