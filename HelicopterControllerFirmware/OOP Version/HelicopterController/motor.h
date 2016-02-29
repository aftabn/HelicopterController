// motorEnums.h

#ifndef _MOTOR_h
#define _MOTOR_h

namespace Motor
{
	enum class Direction { Clockwise, CounterClockwise };
	enum class MotorDriverType { AnalogVoltage, Frequency };

	static const double DBL_MotorIdleVoltage = 2.5;
	static const double DBL_MotorVoltageRange = 2.5;
	static const double DBL_MotorVoltageMin = 0.0;
	static const double DBL_MotorVoltageMax = 5.0;

	static const int INT_MotorMinFrequency = 0;
	static const int INT_MotorMaxFrequency = 30000;
}

#endif
