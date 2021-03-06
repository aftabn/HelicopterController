﻿using log4net;
using System;

namespace Helicopter.Core.Controller
{
    public static class Microcontroller
    {
        private const string STR_IdentityCommand = "*IDN?";
        private const string STR_FirmwareVersionCommand = "VER";
        private const string STR_ChangeLogCommand = "CHANGELOG";
        private const string STR_ProportionalGainCommand = "P";
        private const string STR_IntegralGainCommand = "I";
        private const string STR_DerivativeGainCommand = "D";
        private const string STR_IntegralWindupCommand = "IW";
        private const string STR_OutputRateLimitCommand = "RL";
        private const string STR_PidLoopIntervalCommand = "LI";
        private const string STR_MotorOutputCommand = "O";
        private const string STR_AngleSetPointCommand = "SP";
        private const string STR_AngleCommand = "A";
        private const string STR_ZeroEncoderAngleCommand = "ZE";
        private const string STR_DirectionCommand = "DC";
        private const string STR_MotorDriverCommand = "DV";
        private const string STR_DacVoltageCommand = "DAC";
        private const string STR_FrequencyOutputCommand = "F";
        private const string STR_AdcReadCommand = "ADC";
        private const string STR_PidControlCommand = "PID";
        private const string STR_TiltOutputOffsetCommand = "TOO";
        private const string STR_MaxFrequencyCommand = "MF";
        private const string STR_SafetyCommand = "SAFETY";
        private const string STR_VerboseCommand = "VERBOSE";

        private const string STR_OnArg = "ON";
        private const string STR_OffArg = "OFF";

        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static CommunicationsManager communicationsManager;

        public static bool IsConnected => communicationsManager?.IsConnected ?? false;

        public static void Initialize(ConnectionType connectionType)
        {
            communicationsManager = new CommunicationsManager(connectionType);
        }

        public static void Connect()
        {
            if (!IsConnected)
            {
                communicationsManager.Connect();
            }
            else
            {
                throw new Exception("Microcontroller is already connected.");
            }
        }

        public static void Disconnect()
        {
            if (IsConnected)
            {
                communicationsManager.Disconnect();
            }
            else
            {
                throw new Exception("Microcontroller is already disconnected.");
            }
        }

        public static CommunicationsManager GetInstanceOfCommunicationManager()
        {
            return communicationsManager;
        }

        public static void EnableSafety()
        {
            string command = $"{STR_SafetyCommand} {STR_OnArg}";
            communicationsManager.Write(command);
        }

        public static void DisableSafety()
        {
            string command = $"{STR_SafetyCommand} {STR_OffArg}";
            communicationsManager.Write(command);
        }

        public static void EnablePid()
        {
            string command = $"{STR_PidControlCommand} {STR_OnArg}";
            communicationsManager.Write(command);
        }

        public static void DisablePid()
        {
            string command = $"{STR_PidControlCommand} {STR_OffArg}";
            communicationsManager.Write(command);
        }

        public static int GetMaxFrequency()
        {
            return Convert.ToInt32(communicationsManager.Write(STR_MaxFrequencyCommand).ReturnValue);
        }

        public static void SetMaxFrequency(int frequency)
        {
            string command = $"{STR_MaxFrequencyCommand} {frequency}";
            communicationsManager.Write(command);
        }

        public static int GetTiltOutputOffset()
        {
            return Convert.ToInt32(communicationsManager.Write(STR_TiltOutputOffsetCommand).ReturnValue);
        }

        public static void SetTiltOutputOffset(int tiltOutputOffset)
        {
            string command = $"{STR_TiltOutputOffsetCommand} {tiltOutputOffset}";
            communicationsManager.Write(command);
        }

        public static string GetControllerIdentity()
        {
            return communicationsManager.Write(STR_IdentityCommand).ReturnValue;
        }

        public static double GetFirmwareVersion()
        {
            return Convert.ToDouble(communicationsManager.Write(STR_FirmwareVersionCommand).ReturnValue);
        }

        public static string GetChangelog()
        {
            return communicationsManager.Write(STR_ChangeLogCommand).ReturnValue;
        }

        public static double GetCurrentAngle(int channel)
        {
            string command = $"{STR_AngleCommand} {channel}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void ZeroEncoderAngle()
        {
            string command = $"{STR_ZeroEncoderAngleCommand}";
            communicationsManager.Write(command);
        }

        public static int GetPidLoopInterval()
        {
            return Convert.ToInt32(communicationsManager.Write(STR_PidLoopIntervalCommand).ReturnValue);
        }

        public static void SetPidLoopInterval(int interval)
        {
            string command = $"{STR_PidLoopIntervalCommand} {interval}";
            communicationsManager.Write(command);
        }

        public static double GetAngleSetPoint(int channel)
        {
            string command = $"{STR_AngleSetPointCommand} {channel}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetAngleSetPoint(int channel, double setPoint)
        {
            string command = $"{STR_AngleSetPointCommand} {channel} {setPoint}";
            communicationsManager.Write(command);
        }

        public static int GetMotorOutput(int channel)
        {
            string command = $"{STR_MotorOutputCommand} {channel}";
            return Convert.ToInt32(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetMotorOutput(int channel, int output)
        {
            string command = $"{STR_MotorOutputCommand} {channel} {output}";
            communicationsManager.Write(command);
        }

        public static double GetProportionalGain(int channel, int profile)
        {
            string command = $"{STR_ProportionalGainCommand} {channel} {profile}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetProportionalGain(int channel, int profile, double pGain)
        {
            string command = $"{STR_ProportionalGainCommand} {channel} {profile} {pGain}";
            communicationsManager.Write(command);
        }

        public static double GetIntegralGain(int channel, int profile)
        {
            string command = $"{STR_IntegralGainCommand} {channel} {profile}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetIntegralGain(int channel, int profile, double iGain)
        {
            string command = $"{STR_IntegralGainCommand} {channel} {profile} {iGain}";
            communicationsManager.Write(command);
        }

        public static double GetDerivativeGain(int channel, int profile)
        {
            string command = $"{STR_DerivativeGainCommand} {channel} {profile}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetDerivativeGain(int channel, int profile, double dGain)
        {
            string command = $"{STR_DerivativeGainCommand} {channel} {profile} {dGain}";
            communicationsManager.Write(command);
        }

        public static double GetIntegralWindupThreshold(int channel)
        {
            string command = $"{STR_IntegralWindupCommand} {channel}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetIntegralWindupThreshold(int channel, double windup)
        {
            string command = $"{STR_IntegralWindupCommand} {channel} {windup}";
            communicationsManager.Write(command);
        }

        public static int GetOutputRateLimit(int channel)
        {
            string command = $"{STR_OutputRateLimitCommand} {channel}";
            return Convert.ToInt32(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetOutputRateLimit(int channel, int outputRateLimit)
        {
            string command = $"{STR_OutputRateLimitCommand} {channel} {outputRateLimit}";
            communicationsManager.Write(command);
        }

        public static Direction GetMotorDirection(int channel)
        {
            string command = $"{STR_DirectionCommand} {channel}";
            var direction = communicationsManager.Write(command).ReturnValue;

            switch (direction)
            {
                case "CW":
                    return Direction.Clockwise;

                case "CCW":
                    return Direction.CounterClockwise;

                default:
                    throw new Exception($"Returned direction [{direction}] is not valid");
            }
        }

        public static void SetMotorDirection(int channel, Direction direction)
        {
            string command = $"{STR_DirectionCommand} {channel} {direction}";
            communicationsManager.Write(command);
        }

        public static MotorDriver GetMotorDriver(int channel)
        {
            string command = $"{STR_MotorDriverCommand} {channel}";
            var motorDriver = communicationsManager.Write(command).ReturnValue;

            switch (motorDriver)
            {
                case "AV":
                    return MotorDriver.AnalogVoltage;

                case "F":
                    return MotorDriver.Frequency;

                default:
                    throw new Exception($"Returned motor driver type [{motorDriver}] is not valid");
            }
        }

        public static void SetMotorDriver(int channel, MotorDriver motorDriver)
        {
            string command = $"{STR_MotorDriverCommand} {channel} {motorDriver}";
            communicationsManager.Write(command);
        }

        public static double GetDacVoltage(int channel)
        {
            string command = $"{STR_DacVoltageCommand} {channel}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetDacVoltage(int channel, double voltage)
        {
            string command = $"{STR_DacVoltageCommand} {channel} {voltage}";
            communicationsManager.Write(command);
        }

        public static double GetAdcVoltage(int channel)
        {
            string command = $"{STR_AdcReadCommand} {channel}";
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static double GetFrequency()
        {
            return Convert.ToDouble(communicationsManager.Write(STR_FrequencyOutputCommand).ReturnValue);
        }

        public static void SetFrequency(int frequency)
        {
            string command = $"{STR_FrequencyOutputCommand} {frequency}";
            communicationsManager.Write(command);
        }

        public static void DisableVerboseMode()
        {
            string command = $"{STR_VerboseCommand} {STR_OffArg}";
            communicationsManager.Write(command);
        }
    }
}