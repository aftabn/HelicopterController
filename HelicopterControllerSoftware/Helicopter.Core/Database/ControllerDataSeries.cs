using Helicopter.Core.Controller;
using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace Helicopter.Core.Database
{
    public class ControllerDataSeries : INotifyPropertyChanged, IDisposable
    {
        private AngleController angleController;
        private readonly string controllerDataString;

        public ControllerDataSeries(AngleController angleController)
        {
            controllerDataString = $"{angleController.MotorType}ControllerData";

            this.angleController = angleController;
            angleController.PropertyChanged += OnAngleControllerPropertyChanged;

            ControllerData = angleController.ControllerData;

            MotorType = angleController.MotorType;
            MotorDriver = angleController.MotorDriver;

            foreach (var profile in angleController.PidProfiles.Values)
            {
                if (profile.DirectionProfile == DirectionProfile.CW)
                {
                    CWProportionalGain = profile.ProportionalGain;
                    CWIntegralGain = profile.IntegralGain;
                    CWDerivativeGain = profile.DerivativeGain;
                }
                else
                {
                    CCWProportionalGain = profile.ProportionalGain;
                    CCWIntegralGain = profile.IntegralGain;
                    CCWDerivativeGain = profile.DerivativeGain;
                }
            }

            IWindupThreshold = angleController.IntegralWindupThreshold;
            OutputRateLimit = angleController.OutputRateLimit;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public List<ControllerDataPoint> ControllerData { get; set; }

        public MotorType MotorType { get; set; }

        public MotorDriver MotorDriver { get; set; }

        public double CWProportionalGain { get; set; }

        public double CWIntegralGain { get; set; }

        public double CWDerivativeGain { get; set; }

        public double CCWProportionalGain { get; set; }

        public double CCWIntegralGain { get; set; }

        public double CCWDerivativeGain { get; set; }

        public double IWindupThreshold { get; set; }

        public int OutputRateLimit { get; set; }

        public void Dispose()
        {
            angleController.PropertyChanged -= OnAngleControllerPropertyChanged;
            angleController = null;
        }

        private void OnAngleControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == controllerDataString)
            {
                RaisePropertyChanged(e.PropertyName);
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}