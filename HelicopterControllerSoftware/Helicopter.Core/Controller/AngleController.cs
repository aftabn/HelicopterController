using Helicopter.Core.Sessions;
using Helicopter.Core.Settings;
using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace Helicopter.Core.Controller
{
    public abstract class AngleController : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private int controllerChannel;
        private MotorType motorType;
        private Direction direction;
        private MotorDriver motorDriver;
        private bool isEnabled;
        private double proportionalGain;
        private double integralGain;
        private double derivativeGain;
        private double integralWindupThreshold;
        private int outputRateLimit;
        private double setPoint;
        private double currentAngle;
        private int outputPercentage;

        public AngleController(int controllerChannel)
        {
            this.controllerChannel = controllerChannel;
            this.motorType = (MotorType)controllerChannel;

            ControllerData = new List<ControllerData>();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public List<ControllerData> ControllerData { get; set; }

        public MotorType MotorType { get; protected set; }

        public double ProportionalGain
        {
            get
            {
                return proportionalGain;
            }

            set
            {
                if (value != proportionalGain)
                {
                    proportionalGain = value;
                    RaisePropertyChanged("ProportionalGain");
                }
            }
        }

        public double IntegralGain
        {
            get
            {
                return integralGain;
            }

            set
            {
                if (value != integralGain)
                {
                    integralGain = value;
                    RaisePropertyChanged("IntegralGain");
                }
            }
        }

        public double DerivativeGain
        {
            get
            {
                return derivativeGain;
            }

            set
            {
                if (value != derivativeGain)
                {
                    derivativeGain = value;
                    RaisePropertyChanged("DerivativeGain");
                }
            }
        }

        public double IntegralWindupThreshold
        {
            get
            {
                return integralWindupThreshold;
            }

            set
            {
                if (value != integralWindupThreshold)
                {
                    integralWindupThreshold = value;
                    RaisePropertyChanged("IntegralWindupThreshold");
                }
            }
        }

        public int OutputRateLimit
        {
            get
            {
                return outputRateLimit;
            }

            set
            {
                if (value != outputRateLimit)
                {
                    outputRateLimit = value;
                    RaisePropertyChanged("OutputRateLimit");
                }
            }
        }

        public double SetPoint
        {
            get
            {
                return setPoint;
            }

            set
            {
                if (value != setPoint)
                {
                    setPoint = value;
                    RaisePropertyChanged("SetPoint");
                }
            }
        }

        public double CurrentAngle
        {
            get
            {
                return currentAngle;
            }

            private set
            {
                if (value != currentAngle)
                {
                    currentAngle = value;
                    RaisePropertyChanged("CurrentAngle");
                }
            }
        }

        public int OutputPercentage
        {
            get
            {
                return outputPercentage;
            }

            set
            {
                if (value != outputPercentage)
                {
                    outputPercentage = value;
                    RaisePropertyChanged("OutputPercentage");
                }
            }
        }

        public Direction Direction
        {
            get
            {
                return direction;
            }

            set
            {
                if (value != direction)
                {
                    direction = value;
                    RaisePropertyChanged("Direction");
                }
            }
        }

        public MotorDriver MotorDriver
        {
            get
            {
                return motorDriver;
            }

            set
            {
                if (value != motorDriver)
                {
                    motorDriver = value;
                    RaisePropertyChanged("MotorDriver");
                }
            }
        }

        public bool IsEnabled
        {
            get
            {
                return isEnabled;
            }

            set
            {
                if (value != isEnabled)
                {
                    isEnabled = value;
                    RaisePropertyChanged("IsEnabled");
                }
            }
        }

        public void Initialize()
        {
            RefreshCurrentAngle();
            RefreshOutputPercentage();
            RefreshDirection();
            RefreshProportionalGain();
            RefreshIntegralGain();
            RefreshDerivativeGain();
            RefreshIntegralWindupThreshold();
            RefreshSetPoint();
            RefreshMotorDriver();
        }

        public void LoadSettings(AngleControllerSettings settings)
        {
            IsEnabled = settings.IsEnabled;
            SetMotorDriver(settings.MotorDriver);
            SetProportionalGain(settings.ProportionalGain);
            SetIntegralGain(settings.IntegralGain);
            SetDerivativeGain(settings.DerivativeGain);
            SetIntegralWindupThreshold(settings.IntegralWindupThreshold);
            SetOutputRateLimit(settings.OutputRateLimit);
        }

        public void Dispose()
        {
        }

        public void Disable()
        {
            SetOutputPercentage(0);
        }

        public void RefreshValues()
        {
            RefreshCurrentAngle();
            RefreshOutputPercentage();
            RefreshDirection();
        }

        public void RefreshCurrentAngle()
        {
            CurrentAngle = Microcontroller.GetCurrentAngle(controllerChannel);
        }

        public void RefreshOutputPercentage()
        {
            OutputPercentage = Microcontroller.GetMotorOutput(controllerChannel);
        }

        public void RefreshDirection()
        {
            Direction = Microcontroller.GetMotorDirection(controllerChannel);
        }

        public void RefreshProportionalGain()
        {
            ProportionalGain = Microcontroller.GetProportionalGain(controllerChannel);
        }

        public void RefreshIntegralGain()
        {
            IntegralGain = Microcontroller.GetIntegralGain(controllerChannel);
        }

        public void RefreshDerivativeGain()
        {
            DerivativeGain = Microcontroller.GetDerivativeGain(controllerChannel);
        }

        public void RefreshIntegralWindupThreshold()
        {
            IntegralWindupThreshold = Microcontroller.GetIntegralWindupThreshold(controllerChannel);
        }

        public void RefreshOutputRateLimit()
        {
            OutputRateLimit = Microcontroller.GetOutputRateLimit(controllerChannel);
        }

        public void RefreshSetPoint()
        {
            SetPoint = Microcontroller.GetAngleSetPoint(controllerChannel);
        }

        public void RefreshMotorDriver()
        {
            MotorDriver = Microcontroller.GetMotorDriver(controllerChannel);
        }

        public void SetOutputPercentage(int output)
        {
            Microcontroller.SetMotorOutput(controllerChannel, output);
            OutputPercentage = output;
        }

        public void SetDirection(Direction direction)
        {
            Microcontroller.SetMotorDirection(controllerChannel, direction);
            Direction = direction;
        }

        public void SetProportionalGain(double pGain)
        {
            Microcontroller.SetProportionalGain(controllerChannel, pGain);
            ProportionalGain = pGain;
        }

        public void SetIntegralGain(double iGain)
        {
            Microcontroller.SetIntegralGain(controllerChannel, iGain);
            IntegralGain = iGain;
        }

        public void SetDerivativeGain(double dGain)
        {
            Microcontroller.SetDerivativeGain(controllerChannel, dGain);
            DerivativeGain = dGain;
        }

        public void SetIntegralWindupThreshold(double iWindupThreshold)
        {
            Microcontroller.SetIntegralWindupThreshold(controllerChannel, iWindupThreshold);
            IntegralWindupThreshold = iWindupThreshold;
        }

        public void SetOutputRateLimit(int outputRateLimit)
        {
            Microcontroller.SetOutputRateLimit(controllerChannel, outputRateLimit);
            OutputRateLimit = outputRateLimit;
        }

        public void SetSetPoint(double setPoint)
        {
            Microcontroller.SetAngleSetPoint(controllerChannel, setPoint);
            SetPoint = setPoint;
        }

        public void SetMotorDriver(MotorDriver motorDriver)
        {
            Microcontroller.SetMotorDriver(controllerChannel, motorDriver);
            MotorDriver = motorDriver;
        }

        public void TakeNewDataSample(DateTime timeStamp)
        {
            var data = new ControllerData
            {
                TimeStamp = timeStamp,
                SetPoint = SetPoint,
                CurrentAngle = CurrentAngle,
                ProportionalGain = ProportionalGain,
                IntegralGain = IntegralGain,
                DerivativeGain = DerivativeGain,
                IntegralWindupThreshold = IntegralWindupThreshold,
                OutputRateLimit = OutputRateLimit
            };
        }

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}