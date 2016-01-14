// pidControl.h

#ifndef _PIDCONTROL_h
#define _PIDCONTROL_h

#include "arduino.h"

#define P_GAIN_MIN					0.01
#define P_GAIN_MAX					10.0

#define I_GAIN_MIN					0.0
#define I_GAIN_MAX					20.0
#define I_GAIN_THRESHHOLD_ERROR		10

#define D_GAIN_MIN					0.0
#define D_GAIN_MAX					10.0

#define PWM_DUTY_INTERVALS			2
#define PWM_FREQUENCY				1

#define PWM_DUTY_MIN				0
#define PWM_DUTY_MAX				100

#define SET_POINT_MIN				0.0
#define SET_POINT_MAX				360.0

#define PID_INTERVAL_SEC_MIN		0.1
#define PID_INTERVAL_SEC_MAX		5.0

#define DEFAULT_P_GAIN				P_GAIN_MIN
#define DEFAULT_I_GAIN				I_GAIN_MIN
#define DEFAULT_D_GAIN				D_GAIN_MIN
#define DEFAULT_SET_POINT			SET_POINT_MIN
#define DEFAULT_PID_INTERVAL_SEC	PID_INTERVAL_SEC_MIN

extern volatile bool isPidEnabled;
extern volatile bool isDebugMode;

extern volatile double pGain;
extern volatile double iGain;
extern volatile double dGain;
extern volatile int loopInterval;
extern volatile double setPoint;
extern volatile uint8_t currentDutyCycle;

int getNewPidDutyCycle(void);
int adjustDutyToAllowableValue(int duty);
void setPwmDutyCycle(int duty);

void initializePwm(void);

void initializePid(void);
void enablePid(void);
void disablePid(void);

#endif
