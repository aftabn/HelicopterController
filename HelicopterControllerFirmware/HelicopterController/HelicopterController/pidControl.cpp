//
//
//

#include "util.h"
#include "pidControl.h"

volatile bool isPidEnabled;
volatile bool isDebugMode;

volatile double pGain;
volatile double iGain;
volatile double dGain;
volatile int loopInterval;
volatile double setPoint;
volatile uint8_t currentDutyCycle;

void initializePwm(void)
{
	send("PWM init not implemented");
}

void initializePid(void)
{
	pGain = DEFAULT_P_GAIN;
	iGain = DEFAULT_I_GAIN;
	dGain = DEFAULT_D_GAIN;
	loopInterval = PID_INTERVAL_SEC_MIN;
	setPoint = DEFAULT_SET_POINT;
}

int getNewPidDutyCycle()
{
	return PWM_DUTY_MIN;
}

int adjustDutyToAllowableValue(int duty)
{
	return PWM_DUTY_MIN;
}

void setPwmDutyCycle(int duty)
{
}

void enablePid(void)
{
	isPidEnabled = true;
}

void disablePid(void)
{
	isPidEnabled = false;
}