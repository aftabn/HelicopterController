/*
Name: frequencyGenerator.cpp
Created: 2/25/2016
Author:	Aftab
*/

#ifndef _FREQUENCYGENERATOR_h
#define _FREQUENCYGENERATOR_h

class FrequencyGenerator
{
public:
	FrequencyGenerator();
	~FrequencyGenerator();

	volatile int currentFrequency;

	void initialize(void);
	void setFrequency(int frequency);
	void disable(void);
};

#endif
