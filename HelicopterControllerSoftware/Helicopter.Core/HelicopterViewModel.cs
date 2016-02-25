using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Libs.Utilities;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;

namespace Helicopter.Core
{
    public class HelicopterViewModel : INotifyPropertyChanged, IDisposable
    {
        private HelicopterManager helicopterManager;
        private HelicopterController helicopterController;
        private bool isDeveloperMode;
        private string outputText;

        public HelicopterViewModel(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;

            helicopterManager = new HelicopterManager();
            helicopterController = helicopterManager.HelicopterController;

            AngleControllers = new Dictionary<MotorType, AngleController>();
            AngleControllers.Add(MotorType.Yaw, helicopterController.Yaw);
            AngleControllers.Add(MotorType.Tilt, helicopterController.Tilt);

            OutputText = String.Empty;

            InitializeRelayCommands();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Dictionary<MotorType, AngleController> AngleControllers { get; set; }

        public bool IsDeveloperMode { get { return isDeveloperMode; } }

        public bool IsConnected { get { return helicopterController.IsConnected; } }

        public bool IsPidEnabled { get { return helicopterController.IsPidEnabled; } }

        public bool IsSafetyEnabled { get { return helicopterController.IsSafetyEnabled; } }

        public string ControllerIdentity { get { return helicopterController.ControllerIdentity; } }

        public double FirmwareVersion { get { return helicopterController.FirmwareVersion; } }

        public string Changelog { get { return helicopterController.Changelog; } }

        public double PidLoopInterval { get { return helicopterController.PidLoopInterval; } }

        public double YawCurrentAngle
        {
            get { return AngleControllers[MotorType.Yaw].CurrentAngle; }
        }

        public double YawSetPoint
        {
            get { return AngleControllers[MotorType.Yaw].SetPoint; }
            set { AngleControllers[MotorType.Yaw].SetSetPoint(value); }
        }

        public double YawProportionalGain
        {
            get { return AngleControllers[MotorType.Yaw].ProportionalGain; }
            set { AngleControllers[MotorType.Yaw].SetProportionalGain(value); }
        }

        public double YawIntegralGain
        {
            get { return AngleControllers[MotorType.Yaw].IntegralGain; }
            set { AngleControllers[MotorType.Yaw].SetIntegralGain(value); }
        }

        public double YawDerivativeGain
        {
            get { return AngleControllers[MotorType.Yaw].DerivativeGain; }
            set { AngleControllers[MotorType.Yaw].SetDerivativeGain(value); }
        }

        public double YawIntegralWindupThreshold
        {
            get { return AngleControllers[MotorType.Yaw].IntegralWindupThreshold; }
            set { AngleControllers[MotorType.Yaw].SetIntegralWindupThreshold(value); }
        }

        public int YawOutputRateLimit
        {
            get { return AngleControllers[MotorType.Yaw].OutputRateLimit; }
            set { AngleControllers[MotorType.Yaw].SetOutputRateLimit(value); }
        }

        public int YawOutputPercentage
        {
            get { return AngleControllers[MotorType.Yaw].OutputPercentage; }
            set { AngleControllers[MotorType.Yaw].SetOutputPercentage(value); }
        }

        public Direction YawDirection
        {
            get { return AngleControllers[MotorType.Yaw].Direction; }
            set { AngleControllers[MotorType.Yaw].SetDirection(value); }
        }

        public MotorDriver YawMotorDriver
        {
            get { return AngleControllers[MotorType.Yaw].MotorDriver; }
            set { AngleControllers[MotorType.Yaw].SetMotorDriver(value); }
        }

        public double TiltCurrentAngle
        {
            get { return AngleControllers[MotorType.Tilt].CurrentAngle; }
        }

        public double TiltSetPoint
        {
            get { return AngleControllers[MotorType.Tilt].SetPoint; }
            set { AngleControllers[MotorType.Tilt].SetSetPoint(value); }
        }

        public double TiltProportionalGain
        {
            get { return AngleControllers[MotorType.Tilt].ProportionalGain; }
            set { AngleControllers[MotorType.Tilt].SetProportionalGain(value); }
        }

        public double TiltIntegralGain
        {
            get { return AngleControllers[MotorType.Tilt].IntegralGain; }
            set { AngleControllers[MotorType.Tilt].SetIntegralGain(value); }
        }

        public double TiltDerivativeGain
        {
            get { return AngleControllers[MotorType.Tilt].DerivativeGain; }
            set { AngleControllers[MotorType.Tilt].SetDerivativeGain(value); }
        }

        public double TiltIntegralWindupThreshold
        {
            get { return AngleControllers[MotorType.Tilt].IntegralWindupThreshold; }
            set { AngleControllers[MotorType.Tilt].SetIntegralWindupThreshold(value); }
        }

        public int TiltOutputRateLimit
        {
            get { return AngleControllers[MotorType.Tilt].OutputRateLimit; }
            set { AngleControllers[MotorType.Tilt].SetOutputRateLimit(value); }
        }

        public int TiltOutputPercentage
        {
            get { return AngleControllers[MotorType.Tilt].OutputPercentage; }
            set { AngleControllers[MotorType.Tilt].SetOutputPercentage(value); }
        }

        public Direction TiltDirection
        {
            get { return AngleControllers[MotorType.Tilt].Direction; }
            set { AngleControllers[MotorType.Tilt].SetDirection(value); }
        }

        public MotorDriver TiltMotorDriver
        {
            get { return AngleControllers[MotorType.Tilt].MotorDriver; }
            set { AngleControllers[MotorType.Tilt].SetMotorDriver(value); }
        }

        public SessionData SessionData
        {
            get
            {
                return helicopterManager.Session != null ? helicopterManager.Session.SessionData : null;
            }
        }

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

        public ICommand StartPidSessionCommand { get; private set; }
        public ICommand StopPidSessionCommand { get; private set; }
        public ICommand EnablePidCommand { get; private set; }
        public ICommand DisablePidCommand { get; private set; }
        public ICommand EnableSafetyCommand { get; private set; }
        public ICommand DisableSafetyCommand { get; private set; }
        public ICommand RefreshValuesCommand { get; private set; }
        public ICommand DisableMotorsCommand { get; private set; }

        public ICommand GetYawAngleCommand { get; private set; }
        public ICommand SetYawSetPointCommand { get; private set; }
        public ICommand SetYawProportionalGainCommand { get; private set; }
        public ICommand SetYawIntegralGainCommand { get; private set; }
        public ICommand SetYawDerivativeGainCommand { get; private set; }
        public ICommand SetYawIntegralWindupThresholdCommand { get; private set; }
        public ICommand SetYawOutputRateLimitCommand { get; private set; }
        public ICommand SetYawOutputPercentageCommand { get; private set; }
        public ICommand SetYawDirectionCommand { get; private set; }
        public ICommand SetYawMotorDriverCommand { get; private set; }
        public ICommand GetTiltAngleCommand { get; private set; }
        public ICommand SetTiltSetPointCommand { get; private set; }
        public ICommand SetTiltProportionalGainCommand { get; private set; }
        public ICommand SetTiltIntegralGainCommand { get; private set; }
        public ICommand SetTiltDerivativeGainCommand { get; private set; }
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
            helicopterManager.PropertyChanged += OnHelicopterManagerPropertyChanged;
            helicopterManager.Connect();

            var text = String.Format("Connected to {0}", helicopterController.ControllerIdentity);
            UpdateOutputTextbox(text);
        }

        public void Disconnect()
        {
            helicopterManager.Disconnect();
            helicopterManager.PropertyChanged -= OnHelicopterManagerPropertyChanged;
            UpdateOutputTextbox(String.Format("Disconnected from helicopter controller"));
        }

        public void Dispose()
        {
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

            StartPidSessionCommand = new RelayCommand(
                   x =>
                   {
                       helicopterManager.StartPidSession();
                       UpdateOutputTextbox(String.Format("PID session has started"));
                   },
                   x => IsConnected && !IsPidEnabled && !helicopterManager.IsPidSessionAlive);

            StopPidSessionCommand = new RelayCommand(
                   x =>
                   {
                       helicopterManager.StopPidSession();
                       UpdateOutputTextbox(String.Format("PID session has stopped"));
                   },
                   x => IsConnected && IsPidEnabled && helicopterManager.IsPidSessionAlive);

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
                       helicopterManager.DisableMotors();
                       UpdateOutputTextbox(String.Format("Disabled motors"));
                   },
                   x => IsConnected);

            GetYawAngleCommand = new RelayCommand(
                   x => AngleControllers[MotorType.Yaw].RefreshCurrentAngle(),
                   x => IsConnected);

            SetYawSetPointCommand = new RelayCommand(
                   x =>
                   {
                       var setPoint = Convert.ToDouble(x);
                       YawSetPoint = setPoint;
                   },
                   x => IsConnected);

            SetYawProportionalGainCommand = new RelayCommand(
                   x =>
                   {
                       var pGain = Convert.ToDouble(x);
                       YawProportionalGain = pGain;
                   },
                   x => IsConnected);

            SetYawIntegralGainCommand = new RelayCommand(
                   x =>
                   {
                       var iGain = Convert.ToDouble(x);
                       YawIntegralGain = iGain;
                   },
                   x => IsConnected);

            SetYawDerivativeGainCommand = new RelayCommand(
                   x =>
                   {
                       var dGain = Convert.ToDouble(x);
                       YawDerivativeGain = dGain;
                   },
                   x => IsConnected);

            SetYawIntegralWindupThresholdCommand = new RelayCommand(
                   x =>
                   {
                       var iWindupThreshold = Convert.ToDouble(x);
                       YawIntegralWindupThreshold = iWindupThreshold;
                   },
                   x => IsConnected);

            SetYawOutputPercentageCommand = new RelayCommand(
                   x =>
                   {
                       var output = Convert.ToInt32(x);
                       YawOutputPercentage = output;
                   },
                   x => IsConnected && !IsPidEnabled);

            GetTiltAngleCommand = new RelayCommand(
                   x => AngleControllers[MotorType.Tilt].RefreshCurrentAngle(),
                   x => IsConnected);

            SetTiltSetPointCommand = new RelayCommand(
                   x =>
                   {
                       var setPoint = Convert.ToDouble(x);
                       TiltSetPoint = setPoint;
                   },
                   x => IsConnected);

            SetTiltProportionalGainCommand = new RelayCommand(
                   x =>
                   {
                       var pGain = Convert.ToDouble(x);
                       TiltProportionalGain = pGain;
                   },
                   x => IsConnected);

            SetTiltIntegralGainCommand = new RelayCommand(
                   x =>
                   {
                       var iGain = Convert.ToDouble(x);
                       TiltIntegralGain = iGain;
                   },
                   x => IsConnected);

            SetTiltDerivativeGainCommand = new RelayCommand(
                   x =>
                   {
                       var dGain = Convert.ToDouble(x);
                       TiltDerivativeGain = dGain;
                   },
                   x => IsConnected);

            SetTiltIntegralWindupThresholdCommand = new RelayCommand(
                   x =>
                   {
                       var iWindupThreshold = Convert.ToDouble(x);
                       TiltIntegralWindupThreshold = iWindupThreshold;
                   },
                   x => IsConnected);

            SetTiltOutputPercentageCommand = new RelayCommand(
                   x =>
                   {
                       var output = Convert.ToInt32(x);
                       TiltOutputPercentage = output;
                   },
                   x => IsConnected && !IsPidEnabled);
        }

        private void UpdateOutputTextbox(string text)
        {
            OutputText += String.Format("{0}{1}", text, Environment.NewLine);
            RaisePropertyChanged("OutputText");
        }

        private void OnHelicopterManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
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