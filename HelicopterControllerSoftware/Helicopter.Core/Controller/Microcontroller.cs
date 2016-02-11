using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
        private const string STR_IntegralWindupCommand = "WINDUP";
        private const string STR_OutputRateLimitCommand = "RATE";
        private const string STR_PidLoopIntervalCommand = "LOOP";
        private const string STR_MotorOutputCommand = "OUT";
        private const string STR_AngleSetPointCommand = "SP";
        private const string STR_AngleCommand = "ANGLE";
        private const string STR_DirectionCommand = "DIR";
        private const string STR_MotorDriverCommand = "DRIVER";
        private const string STR_DacVoltageCommand = "DAC";
        private const string STR_FrequencyOutputCommand = "FREQ";
        private const string STR_AdcReadCommand = "ADC";
        private const string STR_PidControlCommand = "PID";
        private const string STR_SafetyCommand = "SAFETY";
        private const string STR_VerboseCommand = "VERBOSE";

        private const string STR_OnArg = "ON";
        private const string STR_OffArg = "OFF";

        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static CommunicationsManager communicationsManager;

        public static bool IsConnected
        {
            get
            {
                return communicationsManager != null ? communicationsManager.IsConnected : false;
            }
        }

        public static void Initialize()
        {
            communicationsManager = new CommunicationsManager();
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
            return communicationsManager != null ? communicationsManager : null;
        }

        public static void EnableSafety()
        {
            string command = String.Format("{0} {1}", STR_SafetyCommand, STR_OnArg);
            communicationsManager.Write(command);
        }

        public static void DisableSafety()
        {
            string command = String.Format("{0} {1}", STR_SafetyCommand, STR_OffArg);
            communicationsManager.Write(command);
        }

        public static void EnablePid()
        {
            string command = String.Format("{0} {1}", STR_PidControlCommand, STR_OnArg);
            communicationsManager.Write(command);
        }

        public static void DisablePid()
        {
            string command = String.Format("{0} {1}", STR_PidControlCommand, STR_OffArg);
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
            string command = String.Format("{0} {1}", STR_AngleCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static int GetPidLoopInterval()
        {
            return Convert.ToInt32(communicationsManager.Write(STR_PidLoopIntervalCommand).ReturnValue);
        }

        public static void SetPidLoopInterval(int interval)
        {
            string command = String.Format("{0} {1}", STR_PidLoopIntervalCommand, interval);
            communicationsManager.Write(command);
        }

        public static double GetAngleSetPoint(int channel)
        {
            string command = String.Format("{0} {1}", STR_AngleSetPointCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetAngleSetPoint(int channel, double setPoint)
        {
            string command = String.Format("{0} {1} {2}", STR_AngleSetPointCommand, channel, setPoint);
            communicationsManager.Write(command);
        }

        public static int GetMotorOutput(int channel)
        {
            string command = String.Format("{0} {1}", STR_MotorOutputCommand, channel);
            return Convert.ToInt32(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetMotorOutput(int channel, int output)
        {
            string command = String.Format("{0} {1} {2}", STR_MotorOutputCommand, channel, output);
            communicationsManager.Write(command);
        }

        public static double GetProportionalGain(int channel)
        {
            string command = String.Format("{0} {1}", STR_ProportionalGainCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetProportionalGain(int channel, double pGain)
        {
            string command = String.Format("{0} {1} {2}", STR_ProportionalGainCommand, channel, pGain);
            communicationsManager.Write(command);
        }

        public static double GetIntegralGain(int channel)
        {
            string command = String.Format("{0} {1}", STR_IntegralGainCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetIntegralGain(int channel, double iGain)
        {
            string command = String.Format("{0} {1} {2}", STR_IntegralGainCommand, channel, iGain);
            communicationsManager.Write(command);
        }

        public static double GetDerivativeGain(int channel)
        {
            string command = String.Format("{0} {1}", STR_DerivativeGainCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetDerivativeGain(int channel, double dGain)
        {
            string command = String.Format("{0} {1} {2}", STR_DerivativeGainCommand, channel, dGain);
            communicationsManager.Write(command);
        }

        public static double GetIntegralWindupThreshold(int channel)
        {
            string command = String.Format("{0} {1}", STR_IntegralWindupCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetIntegralWindupThreshold(int channel, double windup)
        {
            string command = String.Format("{0} {1} {2}", STR_IntegralWindupCommand, channel, windup);
            communicationsManager.Write(command);
        }

        public static int GetOutputRateLimit(int channel)
        {
            string command = String.Format("{0} {1}", STR_OutputRateLimitCommand, channel);
            return Convert.ToInt32(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetOutputRateLimit(int channel, int outputRateLimit)
        {
            string command = String.Format("{0} {1} {2}", STR_OutputRateLimitCommand, channel, outputRateLimit);
            communicationsManager.Write(command);
        }

        public static Direction GetMotorDirection(int channel)
        {
            string command = String.Format("{0} {1}", STR_DirectionCommand, channel);
            string direction = communicationsManager.Write(command).ReturnValue;

            return (Direction)Enum.Parse(typeof(Direction), direction);
        }

        public static void SetMotorDirection(int channel, Direction direction)
        {
            string command = String.Format("{0} {1} {2}", STR_DirectionCommand, channel, direction);
            communicationsManager.Write(command);
        }

        public static MotorDriver GetMotorDriver(int channel)
        {
            string command = String.Format("{0} {1}", STR_MotorDriverCommand, channel);
            string motorDriver = communicationsManager.Write(command).ReturnValue;

            return (MotorDriver)Enum.Parse(typeof(MotorDriver), motorDriver);
        }

        public static void SetMotorDriver(int channel, MotorDriver motorDriver)
        {
            string command = String.Format("{0} {1} {2}", STR_MotorDriverCommand, channel, motorDriver);
            communicationsManager.Write(command);
        }

        public static double GetDacVoltage(int channel)
        {
            string command = String.Format("{0} {1}", STR_DacVoltageCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static void SetDacVoltage(int channel, double voltage)
        {
            string command = String.Format("{0} {1} {2}", STR_DacVoltageCommand, channel, voltage);
            communicationsManager.Write(command);
        }

        public static double GetAdcVoltage(int channel)
        {
            string command = String.Format("{0} {1}", STR_AdcReadCommand, channel);
            return Convert.ToDouble(communicationsManager.Write(command).ReturnValue);
        }

        public static double GetFrequency()
        {
            return Convert.ToDouble(communicationsManager.Write(STR_FrequencyOutputCommand).ReturnValue);
        }

        public static void SetFrequency(int frequency)
        {
            string command = String.Format("{0} {1}", STR_FrequencyOutputCommand, frequency);
            communicationsManager.Write(command);
        }

        public static void DisableVerboseMode()
        {
            string command = String.Format("{0} {1}", STR_VerboseCommand, STR_OffArg);
            communicationsManager.Write(command);
        }
    }
}