using Helicopter.Core.Controller;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Sessions
{
    public class ControllerDataSeries : INotifyPropertyChanged, IDisposable
    {
        private AngleController angleController;
        private string controllerDataString;

        public ControllerDataSeries(AngleController angleController)
        {
            controllerDataString = String.Format("{0}ControllerData", angleController.MotorType);

            this.angleController = angleController;
            angleController.PropertyChanged += OnAngleControllerPropertyChanged;

            Name = angleController.MotorType.ToString();
            ControllerData = angleController.ControllerData;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public List<ControllerDataPoint> ControllerData { get; set; }

        public string Name { get; set; }

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
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}