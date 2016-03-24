using Helicopter.Core.Controller;
using Helicopter.Core.Database;
using Helicopter.Model;
using Libs.Utilities;
using log4net;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Input;

namespace Helicopter.Core
{
    public class HelicopterViewModel : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly HelicopterController helicopterController;
        private readonly YawController yaw;
        private readonly TiltController tilt;
        private ObservableCollection<DatabaseRow> displayedRecords;
        private bool isDatabaseConnected;

        public HelicopterViewModel()
        {
            IsDatabaseConnected = DatabaseManager.IsDatabaseAlive();
            HelicopterManager = new HelicopterManager();
            helicopterController = HelicopterManager.HelicopterController;
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;

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

        public bool IsConnected => helicopterController.IsConnected;

        public ConnectionType ConnectionType
        {
            get { return HelicopterManager.ConnectionType; }
            set { HelicopterManager.ConnectionType = value; }
        }

        public bool IsPidEnabled => helicopterController.IsPidEnabled;

        public bool IsSafetyEnabled => helicopterController.IsSafetyEnabled;

        public string ControllerIdentity => helicopterController.ControllerIdentity;

        public double FirmwareVersion => helicopterController.FirmwareVersion;

        public string Changelog => helicopterController.Changelog;

        public double PidLoopInterval => helicopterController.PidLoopInterval;

        public double YawCurrentAngle => yaw.CurrentAngle;

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

        public double TiltCurrentAngle => tilt.CurrentAngle;

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

        public Session PidSession => HelicopterManager.Session;

        public bool IsPidSessionRunning => HelicopterManager.IsSessionRunning;

        public bool IsPidSessionComplete => HelicopterManager.IsSessionComplete;

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

        public ICommand GetTiltAngleCommand { get; private set; }
        public ICommand SetTiltSetPointCommand { get; private set; }
        public ICommand SetTiltCWProportionalGainCommand { get; private set; }
        public ICommand SetTiltCWIntegralGainCommand { get; private set; }
        public ICommand SetTiltCWDerivativeGainCommand { get; private set; }
        public ICommand SetTiltIntegralWindupThresholdCommand { get; private set; }
        public ICommand SetTiltOutputRateLimitCommand { get; private set; }
        public ICommand SetTiltOutputPercentageCommand { get; private set; }

        // TODO: Implement these on the GUI

        public ICommand SetYawDacVoltageCommand { get; private set; }
        public ICommand SetTiltDacVoltageCommand { get; private set; }
        public ICommand SetFrequencyCommand { get; private set; }

        public void Connect()
        {
            HelicopterManager.PropertyChanged += OnHelicopterManagerPropertyChanged;
            HelicopterManager.Connect();
        }

        public void Disconnect()
        {
            HelicopterManager.Disconnect();
            HelicopterManager.PropertyChanged -= OnHelicopterManagerPropertyChanged;
        }

        public void Dispose()
        {
        }

        public void InitializeDatabase()
        {
            if (!IsDatabaseConnected) return;

            SelectedDate = DateTime.Now;
            DisplayedRecords = DatabaseManager.GetQueriedRecords(IsSearchingById, SelectedRecordId, SelectedDate);
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
                       var text = $"Controller: {helicopterController.ControllerIdentity}{Environment.NewLine}" +
                           $"Firmware Version: {helicopterController.FirmwareVersion}{Environment.NewLine}" +
                           $"Changelog: {helicopterController.Changelog}{Environment.NewLine}";
                       Log.Debug(text);
                   },
                   x => IsConnected);

            SearchForRecordsCommand = new RelayCommand(
                x => DisplayedRecords = DatabaseManager.GetQueriedRecords(IsSearchingById, SelectedRecordId, SelectedDate),
                x => IsDatabaseConnected);

            StartTuningSessionCommand = new RelayCommand(
                   x => HelicopterManager.StartTuningSession(),
                   x => IsConnected && !IsPidEnabled && !IsPidSessionRunning);

            StartDemoSessionCommand = new RelayCommand(
                   x => HelicopterManager.StartDemoSession(),
                   x => IsConnected && !IsPidEnabled && !IsPidSessionRunning);

            StopSessionCommand = new RelayCommand(
                   x => HelicopterManager.StopSession(),
                   x => IsConnected && IsPidEnabled && IsPidSessionRunning);

            EnablePidCommand = new RelayCommand(
                   x => helicopterController.EnablePid(),
                   x => IsConnected && !IsPidEnabled);

            DisablePidCommand = new RelayCommand(
                   x => helicopterController.DisablePid(),
                   x => IsConnected && IsPidEnabled);

            EnableSafetyCommand = new RelayCommand(
                   x => helicopterController.EnableSafety(),
                   x => IsConnected && !IsSafetyEnabled);

            DisableSafetyCommand = new RelayCommand(
                   x => helicopterController.DisableSafety(),
                   x => IsConnected && IsSafetyEnabled);

            RefreshValuesCommand = new RelayCommand(
                    x => helicopterController.RefreshValues(),
                    x => IsConnected);

            DisableMotorsCommand = new RelayCommand(
                    x => HelicopterManager.DisableMotors(),
                    x => IsConnected);

            GetYawAngleCommand = new RelayCommand(
                   x => yaw.RefreshCurrentAngle(),
                   x => IsConnected);

            ZeroYawAngleCommand = new RelayCommand(
                   x => yaw.ZeroAngle(),
                   x => IsConnected && !IsPidEnabled);

            SetYawSetPointCommand = new RelayCommand(
                    x => YawSetPoint = Convert.ToDouble(x),
                    x => IsConnected);

            SetYawOutputPercentageCommand = new RelayCommand(
                   x => YawOutputPercentage = Convert.ToInt32(x),
                   x => IsConnected && !IsPidEnabled);

            SetYawCWProportionalGainCommand = new RelayCommand(
                   x => YawCWProportionalGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawCWIntegralGainCommand = new RelayCommand(
                   x => YawCWIntegralGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawCWDerivativeGainCommand = new RelayCommand(
                   x => YawCWDerivativeGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawCCWProportionalGainCommand = new RelayCommand(
                   x => YawCCWProportionalGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawCCWIntegralGainCommand = new RelayCommand(
                   x => YawCCWIntegralGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawCCWDerivativeGainCommand = new RelayCommand(
                   x => YawCCWDerivativeGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawIntegralWindupThresholdCommand = new RelayCommand(
                   x => YawIntegralWindupThreshold = Convert.ToDouble(x),
                   x => IsConnected);

            SetYawOutputRateLimitCommand = new RelayCommand(
                   x => YawOutputRateLimit = Convert.ToInt32(x),
                   x => IsConnected);

            GetTiltAngleCommand = new RelayCommand(
                   x => tilt.RefreshCurrentAngle(),
                   x => IsConnected);

            SetTiltSetPointCommand = new RelayCommand(
                   x => TiltSetPoint = Convert.ToDouble(x),
                   x => IsConnected);

            SetTiltOutputPercentageCommand = new RelayCommand(
                   x => TiltOutputPercentage = Convert.ToInt32(x),
                   x => IsConnected && !IsPidEnabled);

            SetTiltCWProportionalGainCommand = new RelayCommand(
                   x => TiltCWProportionalGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetTiltCWIntegralGainCommand = new RelayCommand(
                   x => TiltCWIntegralGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetTiltCWDerivativeGainCommand = new RelayCommand(
                   x => TiltCWDerivativeGain = Convert.ToDouble(x),
                   x => IsConnected);

            SetTiltIntegralWindupThresholdCommand = new RelayCommand(
                   x => TiltIntegralWindupThreshold = Convert.ToDouble(x),
                   x => IsConnected);

            SetTiltOutputRateLimitCommand = new RelayCommand(
                   x => TiltOutputRateLimit = Convert.ToInt32(x),
                   x => IsConnected);

            Log.Debug("Finished intializing relay commands");
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
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}