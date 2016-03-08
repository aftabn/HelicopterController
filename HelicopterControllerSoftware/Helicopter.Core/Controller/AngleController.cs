using Helicopter.Core.Sessions;
using Helicopter.Core.Settings;
using log4net;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;

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
        private double integralWindupThreshold;
        private int outputRateLimit;
        private double setPoint;
        private double currentAngle;
        private int outputPercentage;

        public AngleController(int controllerChannel)
        {
            this.controllerChannel = controllerChannel;
            this.motorType = (MotorType)controllerChannel;

            ControllerData = new List<ControllerDataPoint>();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public List<ControllerDataPoint> ControllerData { get; set; }

        public MotorType MotorType { get; protected set; }

        public Dictionary<DirectionProfile, PidProfile> PidProfiles { get; set; }

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

        public void LoadSettings(AngleControllerSettings settings)
        {
            IsEnabled = settings.IsEnabled;
            SetMotorDriver(settings.MotorDriver);
            SetIntegralWindupThreshold(settings.IntegralWindupThreshold);
            SetOutputRateLimit(settings.OutputRateLimit);

            settings.PidProfiles.Values.ToList().ForEach(profile => SetPidValuesFromProfile(profile));
        }

        public void Dispose()
        {
        }

        public void Disable()
        {
            SetOutputPercentage(0);
        }

        public void RefreshAllValues()
        {
            RefreshSetPoint();
            RefreshCurrentAngle();
            RefreshOutputPercentage();
            RefreshDirection();
            RefreshIntegralWindupThreshold();
            RefreshMotorDriver();
            RefreshPidValues();
        }

        public void RefreshProcessValues()
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

        public void RefreshProportionalGain(DirectionProfile profile)
        {
            PidProfiles[profile].ProportionalGain = Microcontroller.GetProportionalGain(controllerChannel, (int)profile);
        }

        public void RefreshIntegralGain(DirectionProfile profile)
        {
            PidProfiles[profile].IntegralGain = Microcontroller.GetIntegralGain(controllerChannel, (int)profile);
        }

        public void RefreshDerivativeGain(DirectionProfile profile)
        {
            PidProfiles[profile].DerivativeGain = Microcontroller.GetDerivativeGain(controllerChannel, (int)profile);
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

        public void SetProportionalGain(DirectionProfile profile, double pGain)
        {
            Microcontroller.SetProportionalGain(controllerChannel, (int)profile, pGain);
            // PidProfiles.Single(x => x.Key == profile).Value.ProportionalGain = pGain;
            PidProfiles[profile].ProportionalGain = pGain;
        }

        public void SetIntegralGain(DirectionProfile profile, double iGain)
        {
            Microcontroller.SetIntegralGain(controllerChannel, (int)profile, iGain);
            PidProfiles[profile].IntegralGain = iGain;
        }

        public void SetDerivativeGain(DirectionProfile profile, double dGain)
        {
            Microcontroller.SetDerivativeGain(controllerChannel, (int)profile, dGain);
            PidProfiles[profile].DerivativeGain = dGain;
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
            RefreshProcessValues();

            var data = new ControllerDataPoint(timeStamp, SetPoint, CurrentAngle);

            ControllerData.Add(data);

            // This is needed in order for PID graphs to update
            RaisePropertyChanged("ControllerData");
        }

        protected void OnPidValuesPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        private void SetPidValuesFromProfile(PidProfile pidProfile)
        {
            var directionProfile = pidProfile.DirectionProfile;

            SetProportionalGain(directionProfile, pidProfile.ProportionalGain);
            SetIntegralGain(directionProfile, pidProfile.IntegralGain);
            SetDerivativeGain(directionProfile, pidProfile.DerivativeGain);
        }

        private void RefreshPidValues()
        {
            foreach (var profile in PidProfiles.Values)
            {
                RefreshProportionalGain(profile.DirectionProfile);
                RefreshIntegralGain(profile.DirectionProfile);
                RefreshDerivativeGain(profile.DirectionProfile);
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(String.Format("{0}{1}", MotorType, propertyName)));
            }
        }
    }
}