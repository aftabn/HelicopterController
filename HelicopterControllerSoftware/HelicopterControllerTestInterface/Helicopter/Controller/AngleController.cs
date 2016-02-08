using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Controller
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
        private double setPoint;
        private double currentAngle;
        private int outputPercentage;

        public AngleController(int controllerChannel)
        {
            this.controllerChannel = controllerChannel;
            this.motorType = (MotorType)controllerChannel;
        }

        public event PropertyChangedEventHandler PropertyChanged;

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
                    Microcontroller.SetProportionalGain(controllerChannel, value);
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
                    Microcontroller.SetIntegralGain(controllerChannel, value);
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
                    Microcontroller.SetDerivativeGain(controllerChannel, value);
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
                    Microcontroller.SetIntegralWindupThreshold(controllerChannel, value);
                    integralWindupThreshold = value;
                    RaisePropertyChanged("IntegralWindupThreshold");
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
                    Microcontroller.SetAngleSetPoint(controllerChannel, value);
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
                    Microcontroller.SetMotorOutput(controllerChannel, value);
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
                    Microcontroller.SetMotorDirection(controllerChannel, value);
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
                    Microcontroller.SetMotorDriver(controllerChannel, value);
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
        }

        public void Initialize()
        {
            proportionalGain = Microcontroller.GetProportionalGain(controllerChannel);
            integralGain = Microcontroller.GetIntegralGain(controllerChannel);
            derivativeGain = Microcontroller.GetDerivativeGain(controllerChannel);
            integralWindupThreshold = Microcontroller.GetIntegralWindupThreshold(controllerChannel);
            setPoint = Microcontroller.GetAngleSetPoint(controllerChannel);
            currentAngle = Microcontroller.GetCurrentAngle(controllerChannel);
            outputPercentage = Microcontroller.GetMotorOutput(controllerChannel);
            direction = Microcontroller.GetMotorDirection(controllerChannel);
            motorDriver = Microcontroller.GetMotorDriver(controllerChannel);

            RaisePropertyChanged("ProportionalGain");
            RaisePropertyChanged("IntegralGain");
            RaisePropertyChanged("DerivativeGain");
            RaisePropertyChanged("IntegralWindupThreshold");
            RaisePropertyChanged("SetPoint");
            RaisePropertyChanged("CurrentAngle");
            RaisePropertyChanged("OutputPercentage");
            RaisePropertyChanged("Direction");
            RaisePropertyChanged("MotorDriver");
        }

        public void Dispose()
        {
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

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}