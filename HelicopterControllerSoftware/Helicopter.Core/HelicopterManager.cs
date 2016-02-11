using Helicopter.Core.Controller;
using Helicopter.Core.Settings;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public class HelicopterManager : INotifyPropertyChanged, IDisposable
    {
        private HelicopterSettings helicopterSettings;

        public HelicopterManager()
        {
            HelicopterController = new HelicopterController();
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            helicopterSettings = HelicopterSettings.Load();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public HelicopterController HelicopterController { get; private set; }

        public bool IsConnected
        {
            get
            {
                return HelicopterController.IsConnected;
            }
        }

        public void Connect()
        {
            HelicopterController.Connect();
            HelicopterController.LoadSettings(helicopterSettings.ControllerSettings);
        }

        public void Disconnect()
        {
            HelicopterController.Disconnect();
        }

        public void Dispose()
        {
        }

        public void DisableMotors()
        {
            HelicopterController.DisableMotors();
        }

        private void OnControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
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