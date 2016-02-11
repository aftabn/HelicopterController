using Helicopter.Core.Settings;
using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Controller
{
    public class HelicopterController : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private CommunicationsManager communicationsManager;
        private bool isSafetyEnabled;
        private bool isPidEnabled;
        private string controllerIdentity;
        private double firmwareVersion;
        private string changelog;
        private int pidLoopInterval;

        public HelicopterController()
        {
            Microcontroller.Initialize();
            communicationsManager = Microcontroller.GetInstanceOfCommunicationManager();
            communicationsManager.PropertyChanged += OnCommunicationManagerPropertyChanged;

            Yaw = new YawController();
            Tilt = new TiltController();

            Yaw.PropertyChanged += OnAngleControllerPropertyChanged;
            Tilt.PropertyChanged += OnAngleControllerPropertyChanged;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsConnected
        {
            get
            {
                return Microcontroller.IsConnected;
            }
        }

        public string ControllerIdentity
        {
            get
            {
                return controllerIdentity;
            }

            private set
            {
                if (value != controllerIdentity)
                {
                    controllerIdentity = value;
                    RaisePropertyChanged("ControllerIdentity");
                }
            }
        }

        public double FirmwareVersion
        {
            get
            {
                return firmwareVersion;
            }

            private set
            {
                if (value != firmwareVersion)
                {
                    firmwareVersion = value;
                    RaisePropertyChanged("FirmwareVersion");
                }
            }
        }

        public string Changelog
        {
            get
            {
                return changelog;
            }

            private set
            {
                if (value != changelog)
                {
                    changelog = value;
                    RaisePropertyChanged("Changelog");
                }
            }
        }

        public bool IsPidEnabled
        {
            get
            {
                return isPidEnabled;
            }

            set
            {
                if (value != isPidEnabled)
                {
                    isPidEnabled = value;
                    RaisePropertyChanged("IsPidEnabled");
                }
            }
        }

        public bool IsSafetyEnabled
        {
            get
            {
                return isSafetyEnabled;
            }

            set
            {
                if (value != isSafetyEnabled)
                {
                    isSafetyEnabled = value;
                    RaisePropertyChanged("IsSafetyEnabled");
                }
            }
        }

        public int PidLoopInterval
        {
            get
            {
                return pidLoopInterval;
            }

            set
            {
                if (value != pidLoopInterval)
                {
                    pidLoopInterval = value;
                    RaisePropertyChanged("PidLoopInterval");
                }
            }
        }

        public YawController Yaw { get; private set; }

        public TiltController Tilt { get; private set; }

        public void Connect()
        {
            if (!IsConnected)
            {
                Microcontroller.Connect();
                InitializeController();
            }
            else
            {
                throw new Exception("Microcontroller is already connected.");
            }
        }

        public void Disconnect()
        {
            Microcontroller.DisablePid();

            if (IsConnected)
            {
                Microcontroller.Disconnect();
            }
            else
            {
                throw new Exception("Microcontroller is already disconnected.");
            }
        }

        public void Dispose()
        {
            Disconnect();
        }

        private void InitializeController()
        {
            DisablePid();
            EnableSafety();

            ControllerIdentity = Microcontroller.GetControllerIdentity();
            FirmwareVersion = Microcontroller.GetFirmwareVersion();
            Changelog = Microcontroller.GetChangelog();

            Yaw.Initialize();
            Tilt.Initialize();

            if (PidLoopInterval != 0)
            {
                PidLoopInterval = Microcontroller.GetPidLoopInterval();
            }
        }

        public void LoadSettings(HelicopterControllerSettings settings)
        {
            if (settings.IsSafetyEnabled)
            {
                EnableSafety();
            }
            else
            {
                DisableSafety();
            }

            SetPidLoopInterval(settings.PidLoopInterval);
            Yaw.LoadSettings(settings.YawControllerSettings);
            Tilt.LoadSettings(settings.TiltControllerSettings);
        }

        public void EnablePid()
        {
            Microcontroller.EnablePid();
            IsPidEnabled = true;
        }

        public void DisablePid()
        {
            Microcontroller.DisablePid();
            IsPidEnabled = false;
        }

        public void EnableSafety()
        {
            Microcontroller.EnableSafety();
            IsSafetyEnabled = true;
        }

        public void DisableSafety()
        {
            Microcontroller.DisableSafety();
            IsSafetyEnabled = false;
        }

        public void RefreshValues()
        {
            Yaw.RefreshValues();
            Tilt.RefreshValues();
        }

        public void DisableMotors()
        {
            Yaw.Disable();
            Tilt.Disable();
        }

        public void RefreshPidLoopInterval()
        {
            PidLoopInterval = Microcontroller.GetPidLoopInterval();
        }

        public void SetPidLoopInterval(int intervalMilliseconds)
        {
            Microcontroller.SetPidLoopInterval(intervalMilliseconds);
            PidLoopInterval = intervalMilliseconds;
        }

        private void OnAngleControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            var angleController = sender as AngleController;
            string propertyChanged = String.Format("{0}{1}", angleController.MotorType, e.PropertyName);

            RaisePropertyChanged(propertyChanged);
        }

        private void OnCommunicationManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
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