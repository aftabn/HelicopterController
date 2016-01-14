//
//
//

#include "util.h"
#include "pidControl.h"

volatile bool isPidEnabled;
volatile bool isDebugMode;

volatile double pGains[MAX_NUM_CHANNELS];
volatile double iGains[MAX_NUM_CHANNELS];
volatile double dGains[MAX_NUM_CHANNELS];
volatile double loopIntervals[MAX_NUM_CHANNELS];
volatile double setPoints[MAX_NUM_CHANNELS];
volatile uint8_t currentDuties[MAX_NUM_CHANNELS];

double measuredAngles[MAX_NUM_CHANNELS];
double angleErrors[MAX_NUM_CHANNELS];
double integratedAngleErrors[MAX_NUM_CHANNELS];
double derivativeAnglesErrors[MAX_NUM_CHANNELS];

void initializePwm(void)
{
}

void initializePid(void)
{
	for (int i = 0; i < MAX_NUM_CHANNELS; i++)
	{
		pGains[i] = DEFAULT_P_GAIN;
		iGains[i] = DEFAULT_I_GAIN;
		dGains[i] = DEFAULT_D_GAIN;
		loopIntervals[i] = PID_INTERVAL_SEC_MIN;
		setPoints[i] = DEFAULT_SET_POINT;
	}
}

int getNewPidDutyCycle(int channel)
{
	double previousAngleError = measuredAngles[channel];
	measuredAngles[channel] = getAngle(channel);
	angleErrors[channel] = setPoints[channel] - measuredAngles[channel];
	integratedAngleErrors[channel] = (abs(angleErrors[channel] < I_GAIN_THRESHHOLD_ERROR) ? integratedAngleErrors[channel] + angleErrors[channel] * loopIntervals[channel] : 0);
	derivativeAnglesErrors[channel] = (measuredAngles[channel] - previousAngleError) / loopIntervals[channel];

	int dutyCycle = (int)(pGains[channel] * angleErrors[channel] + iGains[channel] * integratedAngleErrors[channel] + dGains[channel] * derivativeAnglesErrors[channel]);
	int boundDutyCycle = max(min(dutyCycle, 100), 0);

	return boundDutyCycle;
}

int adjustDutyToAllowableValue(int duty)
{
	return PWM_DUTY_MIN;
}

void setPwmDutyCycle(int channel, int duty)
{
}

//TODO: Update this function
double getAngle(int channel)
{
	return 0.0;
}

void enablePid(void)
{
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