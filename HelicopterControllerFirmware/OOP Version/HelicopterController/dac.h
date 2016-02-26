// dac.h

#ifndef _DAC_h
#define _DAC_h

class Dac
{
private:
	static const int INT_Resolution = 1023;
	static const double DBL_ReferenceVoltage;
	static const double DBL_VoltsPerBit;
	static const double DBL_DefaultVoltage;

	int convertVoltageToDacValue(double voltage);

public:
	Dac() {}
	void initialize(void);
	void setDacVoltage(int channel, double voltage);
};

#endif
