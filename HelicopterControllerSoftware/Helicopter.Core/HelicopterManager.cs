using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Core.Settings;
using System;
using System.ComponentModel;
using System.Threading;

namespace Helicopter.Core
{
    public class HelicopterManager : INotifyPropertyChanged, IDisposable
    {
        private HelicopterSettings helicopterSettings;

        private Thread sessionThread;
        private Session session;

        public HelicopterManager()
        {
            HelicopterController = new HelicopterController();
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            helicopterSettings = HelicopterSettings.Load();
            session = new Session(HelicopterController);
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

        public void StartSession()
        {
            if (!sessionThread.IsAlive)
            {
                session.StartSession();
            }
            else
            {
                throw new Exception("The session thread is currently alive. You can only start a new session once the previous one has finished.");
            }
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