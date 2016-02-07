using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public class HelicopterController : INotifyPropertyChanged, IDisposable
    {
        private const int INT_PidIntervalMilliseconds = 250;
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private YawController yawController;
        private TiltController tiltController;
        private bool isSafetyEnabled;
        private bool isDeveloperMode;
        private bool isPidEnabled;
        private string controllerIdentity;
        private double firmwareVersion;
        private string changelog;
        private int pidLoopInterval;

        public HelicopterController(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;

            Microcontroller.Initialize();

            yawController = new YawController();
            tiltController = new TiltController();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsDeveloperMode
        {
            get
            {
                return isDeveloperMode;
            }
        }

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

        public double YawAngleSetPoint
        {
            get
            {
                return yawController.AngleSetPoint;
            }

            set
            {
                yawController.AngleSetPoint = value;
            }
        }

        public double CurrentYawAngle
        {
            get
            {
                return yawController.CurrentAngle;
            }
        }

        public double TiltAngleSetPoint
        {
            get
            {
                return tiltController.AngleSetPoint;
            }

            set
            {
                yawController.AngleSetPoint = value;
            }
        }

        public double CurrentTiltAngle
        {
            get
            {
                return tiltController.AngleSetPoint;
            }
        }

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
            Microcontroller.DisablePid();
            Microcontroller.EnableSafety();

            ControllerIdentity = Microcontroller.GetControllerIdentity();
            FirmwareVersion = Microcontroller.GetFirmwareVersion();
            Changelog = Microcontroller.GetChangelog();

            yawController.Initialize();
            tiltController.Initialize();

            if (PidLoopInterval != 0)
            {
                PidLoopInterval = Microcontroller.GetPidLoopInterval();
            }
        }

        private void OnComponentPropertyChanged(object sender, PropertyChangedEventArgs e)
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