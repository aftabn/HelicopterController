using Helicopter.Core.Settings;
using log4net;
using System;
using System.ComponentModel;

namespace Helicopter.Core.Controller
{
    public class HelicopterController : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly CommunicationsManager communicationsManager;
        private bool isSafetyEnabled;
        private bool isPidEnabled;
        private string controllerIdentity;
        private double firmwareVersion;
        private string changelog;
        private int pidLoopInterval;

        public HelicopterController(ConnectionType connectionType)
        {
            Microcontroller.Initialize(connectionType);
            communicationsManager = Microcontroller.GetInstanceOfCommunicationManager();
            communicationsManager.PropertyChanged += OnCommunicationManagerPropertyChanged;

            Yaw = new YawController();
            Tilt = new TiltController();

            Yaw.PropertyChanged += OnAngleControllerPropertyChanged;
            Tilt.PropertyChanged += OnAngleControllerPropertyChanged;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsConnected => Microcontroller.IsConnected;

        public ConnectionType ConnectionType
        {
            get { return communicationsManager.ConnectionType; }
            set { communicationsManager.ConnectionType = value; }
        }

        public string ControllerIdentity
        {
            get
            {
                return controllerIdentity;
            }

            private set
            {
                if (value == controllerIdentity) return;

                controllerIdentity = value;
                RaisePropertyChanged("ControllerIdentity");
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
                if (value == firmwareVersion) return;

                firmwareVersion = value;
                RaisePropertyChanged("FirmwareVersion");
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
                if (value == changelog) return;

                changelog = value;
                RaisePropertyChanged("Changelog");
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
                if (value == isPidEnabled) return;

                isPidEnabled = value;
                RaisePropertyChanged("IsPidEnabled");
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
                if (value == isSafetyEnabled) return;

                isSafetyEnabled = value;
                RaisePropertyChanged("IsSafetyEnabled");
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
                if (value == pidLoopInterval) return;

                pidLoopInterval = value;
                RaisePropertyChanged("PidLoopInterval");
            }
        }

        public YawController Yaw { get; }

        public TiltController Tilt { get; }

        public void Connect(HelicopterControllerSettings settings)
        {
            if (!IsConnected)
            {
                Microcontroller.Connect();
                LoadSettings(settings);
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
            Microcontroller.DisableVerboseMode();

            DisablePid();
            EnableSafety();

            ControllerIdentity = Microcontroller.GetControllerIdentity();
            FirmwareVersion = Microcontroller.GetFirmwareVersion();
            Changelog = Microcontroller.GetChangelog();

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

            SetPidLoopInterval(settings.PidLoopIntervalMilliseconds);
            Yaw.LoadSettings(settings.YawControllerSettings);
            Tilt.LoadSettings(settings.TiltControllerSettings);

            Yaw.RefreshAllValues();
            Tilt.RefreshAllValues();
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
            Yaw.RefreshProcessValues();
            Tilt.RefreshProcessValues();
        }

        public void DisableMotors()
        {
            DisablePid();
            Yaw.Disable();
            Tilt.Disable();
        }

        public void ZeroYawAngle()
        {
            Yaw.ZeroAngle();
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
            RaisePropertyChanged(e.PropertyName);
        }

        private void OnCommunicationManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}