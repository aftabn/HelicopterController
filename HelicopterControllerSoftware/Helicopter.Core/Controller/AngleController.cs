﻿using Helicopter.Core.Database;
using Helicopter.Core.Settings;
using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace Helicopter.Core.Controller
{
    public abstract class AngleController : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly int controllerChannel;
        private readonly MotorType motorType;
        private Direction direction;
        private MotorDriver motorDriver;
        private bool isEnabled;
        private double integralWindupThreshold;
        private int outputRateLimit;
        private double setPoint;
        private double currentAngle;
        private int outputPercentage;

        protected AngleController(int controllerChannel)
        {
            this.controllerChannel = controllerChannel;
            this.motorType = (MotorType)controllerChannel;

            ControllerData = new List<ControllerDataPoint>();

            Log.DebugFormat("Created new {0} motor", motorType);
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
                if (value == integralWindupThreshold) return;

                integralWindupThreshold = value;
                RaisePropertyChanged("IntegralWindupThreshold");
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
                if (value == outputRateLimit) return;

                outputRateLimit = value;
                RaisePropertyChanged("OutputRateLimit");
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
                if (value == setPoint) return;

                setPoint = value;
                RaisePropertyChanged("SetPoint");
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
                if (value == currentAngle) return;

                currentAngle = value;
                RaisePropertyChanged("CurrentAngle");
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
                if (value == outputPercentage) return;

                outputPercentage = value;
                RaisePropertyChanged("OutputPercentage");
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
                if (value == direction) return;

                direction = value;
                RaisePropertyChanged("Direction");
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
                if (value == motorDriver) return;

                motorDriver = value;
                RaisePropertyChanged("MotorDriver");
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
                if (value == isEnabled) return;

                isEnabled = value;
                RaisePropertyChanged("IsEnabled");
            }
        }

        public void LoadSettings(AngleControllerSettings settings)
        {
            IsEnabled = settings.IsEnabled;
            SetMotorDriver(settings.MotorDriver);
            SetIntegralWindupThreshold(settings.IntegralWindupThreshold);
            SetOutputRateLimit(settings.OutputRateLimit);

            settings.PidProfiles.Values.ToList().ForEach(SetPidValuesFromProfile);

            Log.DebugFormat("Loaded settings for {0} motor", motorType);
        }

        public void Dispose()
        {
        }

        public void Disable()
        {
            SetOutputPercentage(0);
            Log.DebugFormat("Disabling {0} motor", motorType);
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
            Log.DebugFormat("Refreshing all values for {0} motor", motorType);
        }

        public void RefreshProcessValues()
        {
            RefreshCurrentAngle();
            RefreshOutputPercentage();
            RefreshDirection();
            Log.DebugFormat("Refreshing process values for {0} motor", motorType);
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
            // Appending the motor type is needed to differentiate between Yaw and Tilt
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs($"{MotorType}{propertyName}"));
        }
    }
}