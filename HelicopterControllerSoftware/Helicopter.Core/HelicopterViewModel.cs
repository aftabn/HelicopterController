using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Model;
using Libs.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Threading;
using System.Windows.Input;

namespace Helicopter.Core
{
    public class HelicopterViewModel : INotifyPropertyChanged, IDisposable
    {
        private HelicopterController helicopterController;
        private YawController yaw;
        private TiltController tilt;
        private ObservableCollection<DatabaseRow> displayedRecords;
        private bool isDeveloperMode;
        private bool isDatabaseConnected;
        private string outputText;

        public HelicopterViewModel(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;

            IsDatabaseConnected = DatabaseManager.IsDatabaseAlive();
            HelicopterManager = new HelicopterManager();
            helicopterController = HelicopterManager.HelicopterController;
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;

            OutputText = String.Empty;

            InitializeRelayCommands();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public HelicopterManager HelicopterManager { get; private set; }

        public ObservableCollection<DatabaseRow> DisplayedRecords
        {
            get
            {
                return displayedRecords;
            }

            set
            {
                displayedRecords = value;
                RaisePropertyChanged("DisplayedRecords");
            }
        }

        public DatabaseRow SelectedRecord { get; set; }

        public int SelectedRecordId { get; set; }

        public DateTime SelectedDate { get; set; }

        public bool IsSearchingById { get; set; }

        public bool IsDatabaseConnected
        {
            get
            {
                return isDatabaseConnected;
            }

            set
            {
                if (value != isDatabaseConnected)
                {
                    isDatabaseConnected = value;
                    RaisePropertyChanged("IsDatabaseConnected");
                }
            }
        }

        public bool IsDeveloperMode { get { return isDeveloperMode; } }

        public bool IsConnected { get { return helicopterController.IsConnected; } }

        public ConnectionType ConnectionType
        {
            get { return HelicopterManager.ConnectionType; }
            set { HelicopterManager.ConnectionType = value; }
        }

        public bool IsPidEnabled { get { return helicopterController.IsPidEnabled; } }

        public bool IsSafetyEnabled { get { return helicopterController.IsSafetyEnabled; } }

        public string ControllerIdentity { get { return helicopterController.ControllerIdentity; } }

        public double FirmwareVersion { get { return helicopterController.FirmwareVersion; } }

        public string Changelog { get { return helicopterController.Changelog; } }

        public double PidLoopInterval { get { return helicopterController.PidLoopInterval; } }

        public double YawCurrentAngle
        {
            get { return yaw.CurrentAngle; }
        }

        public double YawSetPoint
        {
            get { return yaw.SetPoint; }
            set { yaw.SetSetPoint(value); }
        }

        public double YawCWProportionalGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CW].ProportionalGain; }
            set { yaw.SetProportionalGain(DirectionProfile.CW, value); }
        }

        public double YawCWIntegralGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CW].IntegralGain; }
            set { yaw.SetIntegralGain(DirectionProfile.CW, value); }
        }

        public double YawCWDerivativeGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CW].DerivativeGain; }
            set { yaw.SetDerivativeGain(DirectionProfile.CW, value); }
        }

        public double YawCCWProportionalGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CCW].ProportionalGain; }
            set { yaw.SetProportionalGain(DirectionProfile.CCW, value); }
        }

        public double YawCCWIntegralGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CCW].IntegralGain; }
            set { yaw.SetIntegralGain(DirectionProfile.CCW, value); }
        }

        public double YawCCWDerivativeGain
        {
            get { return yaw.PidProfiles[DirectionProfile.CCW].DerivativeGain; }
            set { yaw.SetDerivativeGain(DirectionProfile.CCW, value); }
        }

        public double YawIntegralWindupThreshold
        {
            get { return yaw.IntegralWindupThreshold; }
            set { yaw.SetIntegralWindupThreshold(value); }
        }

        public int YawOutputRateLimit
        {
            get { return yaw.OutputRateLimit; }
            set { yaw.SetOutputRateLimit(value); }
        }

        public int YawOutputPercentage
        {
            get { return yaw.OutputPercentage; }
            set { yaw.SetOutputPercentage(value); }
        }

        public Direction YawDirection
        {
            get { return yaw.Direction; }
            set { yaw.SetDirection(value); }
        }

        public MotorDriver YawMotorDriver
        {
            get { return yaw.MotorDriver; }
            set { yaw.SetMotorDriver(value); }
        }

        public double TiltCurrentAngle
        {
            get { return tilt.CurrentAngle; }
        }

        public double TiltSetPoint
        {
            get { return tilt.SetPoint; }
            set { tilt.SetSetPoint(value); }
        }

        public double TiltCWProportionalGain
        {
            get { return tilt.PidProfiles[DirectionProfile.CW].ProportionalGain; }
            set { tilt.SetProportionalGain(DirectionProfile.CW, value); }
        }

        public double TiltCWIntegralGain
        {
            get { return tilt.PidProfiles[DirectionProfile.CW].IntegralGain; }
            set { tilt.SetIntegralGain(DirectionProfile.CW, value); }
        }

        public double TiltCWDerivativeGain
        {
            get { return tilt.PidProfiles[DirectionProfile.CW].DerivativeGain; }
            set { tilt.SetDerivativeGain(DirectionProfile.CW, value); }
        }

        public double TiltIntegralWindupThreshold
        {
            get { return tilt.IntegralWindupThreshold; }
            set { tilt.SetIntegralWindupThreshold(value); }
        }

        public int TiltOutputRateLimit
        {
            get { return tilt.OutputRateLimit; }
            set { tilt.SetOutputRateLimit(value); }
        }

        public int TiltOutputPercentage
        {
            get { return tilt.OutputPercentage; }
            set { tilt.SetOutputPercentage(value); }
        }

        public Direction TiltDirection
        {
            get { return tilt.Direction; }
            set { tilt.SetDirection(value); }
        }

        public MotorDriver TiltMotorDriver
        {
            get { return tilt.MotorDriver; }
            set { tilt.SetMotorDriver(value); }
        }

        public Session PidSession { get { return HelicopterManager.Session; } }

        public bool IsPidSessionRunning { get { return HelicopterManager.IsSessionRunning; } }

        public bool IsPidSessionComplete { get { return HelicopterManager.IsSessionComplete; } }

        public string OutputText
        {
            get
            {
                return outputText;
            }

            set
            {
                outputText = value;
                RaisePropertyChanged("OutputText");
            }
        }

        public ICommand ConnectCommand { get; private set; }
        public ICommand DisconnectCommand { get; private set; }
        public ICommand GetControllerInfoCommand { get; private set; }

        public ICommand SearchForRecordsCommand { get; private set; }

        public ICommand StartTuningSessionCommand { get; private set; }
        public ICommand StartDemoSessionCommand { get; private set; }
        public ICommand StopSessionCommand { get; private set; }
        public ICommand EnablePidCommand { get; private set; }
        public ICommand DisablePidCommand { get; private set; }
        public ICommand EnableSafetyCommand { get; private set; }
        public ICommand DisableSafetyCommand { get; private set; }
        public ICommand RefreshValuesCommand { get; private set; }
        public ICommand DisableMotorsCommand { get; private set; }

        public ICommand GetYawAngleCommand { get; private set; }
        public ICommand ZeroYawAngleCommand { get; private set; }
        public ICommand SetYawSetPointCommand { get; private set; }
        public ICommand SetYawCWProportionalGainCommand { get; private set; }
        public ICommand SetYawCWIntegralGainCommand { get; private set; }
        public ICommand SetYawCWDerivativeGainCommand { get; private set; }
        public ICommand SetYawCCWProportionalGainCommand { get; private set; }
        public ICommand SetYawCCWIntegralGainCommand { get; private set; }
        public ICommand SetYawCCWDerivativeGainCommand { get; private set; }
        public ICommand SetYawIntegralWindupThresholdCommand { get; private set; }
        public ICommand SetYawOutputRateLimitCommand { get; private set; }
        public ICommand SetYawOutputPercentageCommand { get; private set; }
        public ICommand SetYawDirectionCommand { get; private set; }
        public ICommand SetYawMotorDriverCommand { get; private set; }
        public ICommand GetTiltAngleCommand { get; private set; }
        public ICommand SetTiltSetPointCommand { get; private set; }
        public ICommand SetTiltCWProportionalGainCommand { get; private set; }
        public ICommand SetTiltCWIntegralGainCommand { get; private set; }
        public ICommand SetTiltCWDerivativeGainCommand { get; private set; }
        public ICommand SetTiltIntegralWindupThresholdCommand { get; private set; }
        public ICommand SetTiltOutputRateLimitCommand { get; private set; }
        public ICommand SetTiltOutputPercentageCommand { get; private set; }
        public ICommand SetTiltDirectionCommand { get; private set; }
        public ICommand SetTiltMotorDriverCommand { get; private set; }

        public ICommand SetYawDacVoltageCommand { get; private set; }
        public ICommand SetTiltDacVoltageCommand { get; private set; }
        public ICommand SetFrequencyCommand { get; private set; }

        public void Connect()
        {
            HelicopterManager.PropertyChanged += OnHelicopterManagerPropertyChanged;
            HelicopterManager.Connect();

            var text = String.Format("Connected to {0}", helicopterController.ControllerIdentity);
            UpdateOutputTextbox(text);
        }

        public void Disconnect()
        {
            HelicopterManager.Disconnect();
            HelicopterManager.PropertyChanged -= OnHelicopterManagerPropertyChanged;
            UpdateOutputTextbox(String.Format("Disconnected from helicopter controller"));
        }

        public void Dispose()
        {
        }

        public void InitializeDatabase()
        {
            if (IsDatabaseConnected)
            {
                SelectedDate = DateTime.Now;
                DisplayedRecords = DatabaseManager.GetQueriedRecords(IsSearchingById, SelectedRecordId, SelectedDate);
            }
        }

        private void InitializeRelayCommands()
        {
            ConnectCommand = new RelayCommand(
                    x => Connect(),
                    x => !IsConnected);

            DisconnectCommand = new RelayCommand(
                   x => Disconnect(),
                   x => IsConnected);

            GetControllerInfoCommand = new RelayCommand(
                   x =>
                   {
                       var text = String.Format("Controller: {0}{1}Firmware Version: {2}{3}Changelog: {4}{5}",
                            helicopterController.ControllerIdentity, Environment.NewLine, helicopterController.FirmwareVersion,
                            Environment.NewLine, helicopterController.Changelog, Environment.NewLine);

                       UpdateOutputTextbox(text);
                   },
                   x => IsConnected);

            SearchForRecordsCommand = new RelayCommand(
                x => DisplayedRecords = DatabaseManager.GetQueriedRecords(IsSearchingById, SelectedRecordId, SelectedDate),
                x => IsDatabaseConnected);

            StartTuningSessionCommand = new RelayCommand(
                   x =>
                   {
                       HelicopterManager.StartSession();
                       UpdateOutputTextbox(String.Format("PID session has started"));
                   },
                   x => IsConnected && !IsPidEnabled && !IsPidSessionRunning);

            StartDemoSessionCommand = new RelayCommand(
                   x =>
                   {
                       HelicopterManager.StartDemo();
                       UpdateOutputTextbox(String.Format("PID session has started"));
                   },
                   x => IsConnected && !IsPidEnabled && !IsPidSessionRunning);

            StopSessionCommand = new RelayCommand(
                   x =>
                   {
                       HelicopterManager.StopSession();
                       UpdateOutputTextbox(String.Format("PID session has stopped"));
                   },
                   x => IsConnected && IsPidEnabled && IsPidSessionRunning);

            EnablePidCommand = new RelayCommand(
                   x =>
                   {
                       helicopterController.EnablePid();
                       UpdateOutputTextbox(String.Format("PID has been enabled"));
                   },
                   x => IsConnected && !IsPidEnabled);

            DisablePidCommand = new RelayCommand(
                   x =>
                   {
                       helicopterController.DisablePid();
                       UpdateOutputTextbox(String.Format("PID has been disabled"));
                   },
                   x => IsConnected && IsPidEnabled);

            EnableSafetyCommand = new RelayCommand(
                   x =>
                   {
                       helicopterController.EnableSafety();
                       UpdateOutputTextbox(String.Format("Safety has been enabled"));
                   },
                   x => IsConnected && !IsSafetyEnabled);

            DisableSafetyCommand = new RelayCommand(
                   x =>
                   {
                       helicopterController.DisableSafety();
                       UpdateOutputTextbox(String.Format("Safety has been disabled"));
                   },
                   x => IsConnected && IsSafetyEnabled);

            RefreshValuesCommand = new RelayCommand(
                   x =>
                   {
                       helicopterController.RefreshValues();
                       UpdateOutputTextbox(String.Format("Refreshed all controller values"));
                   },
                   x => IsConnected);

            DisableMotorsCommand = new RelayCommand(
                   x =>
                   {
                       HelicopterManager.DisableMotors();
                       UpdateOutputTextbox(String.Format("Disabled motors"));
                   },
                   x => IsConnected);

            GetYawAngleCommand = new RelayCommand(
                   x => yaw.RefreshCurrentAngle(),
                   x => IsConnected);

            ZeroYawAngleCommand = new RelayCommand(
                   x => yaw.ZeroAngle(),
                   x => IsConnected && !IsPidEnabled);

            SetYawSetPointCommand = new RelayCommand(
                   x =>
                   {
                       var setPoint = Convert.ToDouble(x);
                       YawSetPoint = setPoint;
                   },
                   x => IsConnected);

            SetYawOutputPercentageCommand = new RelayCommand(
                   x =>
                   {
                       var output = Convert.ToInt32(x);
                       YawOutputPercentage = output;
                   },
                   x => IsConnected && !IsPidEnabled);

            SetYawCWProportionalGainCommand = new RelayCommand(
                   x =>
                   {
                       var pGain = Convert.ToDouble(x);
                       YawCWProportionalGain = pGain;
                   },
                   x => IsConnected);

            SetYawCWIntegralGainCommand = new RelayCommand(
                   x =>
                   {
                       var iGain = Convert.ToDouble(x);
                       YawCWIntegralGain = iGain;
                   },
                   x => IsConnected);

            SetYawCWDerivativeGainCommand = new RelayCommand(
                   x =>
                   {
                       var dGain = Convert.ToDouble(x);
                       YawCWDerivativeGain = dGain;
                   },
                   x => IsConnected);

            SetYawCCWProportionalGainCommand = new RelayCommand(
                   x =>
                   {
                       var pGain = Convert.ToDouble(x);
                       YawCCWProportionalGain = pGain;
                   },
                   x => IsConnected);

            SetYawCCWIntegralGainCommand = new RelayCommand(
                   x =>
                   {
                       var iGain = Convert.ToDouble(x);
                       YawCCWIntegralGain = iGain;
                   },
                   x => IsConnected);

            SetYawCCWDerivativeGainCommand = new RelayCommand(
                   x =>
                   {
                       var dGain = Convert.ToDouble(x);
                       YawCCWDerivativeGain = dGain;
                   },
                   x => IsConnected);

            SetYawIntegralWindupThresholdCommand = new RelayCommand(
                   x =>
                   {
                       var iWindupThreshold = Convert.ToDouble(x);
                       YawIntegralWindupThreshold = iWindupThreshold;
                   },
                   x => IsConnected);

            SetYawOutputRateLimitCommand = new RelayCommand(
                   x =>
                   {
                       var outputRateLimit = Convert.ToInt32(x);
                       YawOutputRateLimit = outputRateLimit;
                   },
                   x => IsConnected);

            GetTiltAngleCommand = new RelayCommand(
                   x => tilt.RefreshCurrentAngle(),
                   x => IsConnected);

            SetTiltSetPointCommand = new RelayCommand(
                   x =>
                   {
                       var setPoint = Convert.ToDouble(x);
                       TiltSetPoint = setPoint;
                   },
                   x => IsConnected);

            SetTiltOutputPercentageCommand = new RelayCommand(
                   x =>
                   {
                       var output = Convert.ToInt32(x);
                       TiltOutputPercentage = output;
                   },
                   x => IsConnected && !IsPidEnabled);

            SetTiltCWProportionalGainCommand = new RelayCommand(
                   x =>
                   {
                       var pGain = Convert.ToDouble(x);
                       TiltCWProportionalGain = pGain;
                   },
                   x => IsConnected);

            SetTiltCWIntegralGainCommand = new RelayCommand(
                   x =>
                   {
                       var iGain = Convert.ToDouble(x);
                       TiltCWIntegralGain = iGain;
                   },
                   x => IsConnected);

            SetTiltCWDerivativeGainCommand = new RelayCommand(
                   x =>
                   {
                       var dGain = Convert.ToDouble(x);
                       TiltCWDerivativeGain = dGain;
                   },
                   x => IsConnected);

            SetTiltIntegralWindupThresholdCommand = new RelayCommand(
                   x =>
                   {
                       var iWindupThreshold = Convert.ToDouble(x);
                       TiltIntegralWindupThreshold = iWindupThreshold;
                   },
                   x => IsConnected);

            SetTiltOutputRateLimitCommand = new RelayCommand(
                   x =>
                   {
                       var outputRateLimit = Convert.ToInt32(x);
                       TiltOutputRateLimit = outputRateLimit;
                   },
                   x => IsConnected);
        }

        private void UpdateOutputTextbox(string text)
        {
            OutputText += String.Format("{0}{1}", text, Environment.NewLine);
            RaisePropertyChanged("OutputText");
        }

        private void OnHelicopterManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsSessionRequestingStop")
            {
                HelicopterManager.StopSession();
            }
            else
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