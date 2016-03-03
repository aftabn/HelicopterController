using Helicopter.Core.Controller;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Sessions
{
    public class ControllerDataSeries : INotifyPropertyChanged
    {
        private AngleController angleController;

        public ControllerDataSeries(AngleController angleController)
        {
            this.angleController = angleController;
            angleController.PropertyChanged += OnAngleControllerPropertyChanged;

            Name = angleController.MotorType.ToString();
            ControllerData = angleController.ControllerData;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public string Name { get; set; }

        public List<ControllerDataPoint> ControllerData { get; set; }

        public double LastSetPoint
        {
            get
            {
                return ControllerData.Select(x => x.SetPoint).Last();
            }
        }

        public double LastAngle
        {
            get
            {
                return ControllerData.Select(x => x.CurrentAngle).Last();
            }
        }

        private void OnAngleControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ControllerData")
            {
                RaisePropertyChanged(e.PropertyName);
            }
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