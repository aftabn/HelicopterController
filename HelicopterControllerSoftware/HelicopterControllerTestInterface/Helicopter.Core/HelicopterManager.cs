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
            helicopterSettings = HelicopterSettings.Load();
            HelicopterController = new HelicopterController();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public HelicopterController HelicopterController { get; private set; }

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
    }
}