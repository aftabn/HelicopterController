using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HelicopterController.Core
{
    public class Microcontroller : INotifyPropertyChanged, IDisposable
    {
        protected CommunicationsManager communicationsManager;
        protected bool isDebugEnabled;
        protected bool isPidEnabled;
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private double angleSetPoint;
        private double derivativeGain;
        private int dutyCycle;
        private double integralGain;
        private bool isConnected;
        private bool isDeveloperMode;
        private int maximumDutyCycle;
        private int pidLoopInterval;
        private double proportionalGain;

        public Microcontroller(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;
            communicationsManager = new CommunicationsManager(isDeveloperMode);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsConnected
        {
            get
            {
                return communicationsManager != null ? communicationsManager.IsConnected : false;
            }
        }

        public bool IsDebugEnabled
        {
            get
            {
                return isDebugEnabled;
            }
        }

        public bool IsDeveloperMode
        {
            get
            {
                return isDeveloperMode;
            }
        }

        public void Connect()
        {
            if (!IsConnected)
            {
                communicationsManager.Connect();
                InitializeMicrocontroller();
            }
            else
            {
                throw new Exception("Microcontroller is already connected.");
            }
        }

        public void Disconnect()
        {
            DisablePid();

            if (IsConnected)
            {
                communicationsManager.Disconnect();
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

        public void EnableDebugMode()
        {
            communicationsManager.Write("DEBUG ON");
            isDebugEnabled = true;
            RaisePropertyChanged("IsDebugEnabled");
        }

        public void DisableDebugMode()
        {
            communicationsManager.Write("DEBUG OFF");
            isDebugEnabled = false;
            RaisePropertyChanged("IsDebugEnabled");
        }

        public void EnablePid()
        {
            communicationsManager.Write("PID ON");
            isPidEnabled = true;
            RaisePropertyChanged("IsPidEnabled");
        }

        public void DisablePid()
        {
            communicationsManager.Write("PID OFF");
            isPidEnabled = false;
            RaisePropertyChanged("IsPidEnabled");
        }

        public string GetControllerIdentity()
        {
            return communicationsManager.Write("*IDN?").ReturnValue;
        }

        public double GetFirmwareVersion()
        {
            return Convert.ToDouble(communicationsManager.Write("VER").ReturnValue);
        }

        public double GetAngleSetPoint(int channel)
        {
            return Convert.ToDouble(communicationsManager.Write("SP").ReturnValue);
        }

        public int GetCurrentOutput()
        {
            return Convert.ToInt32(communicationsManager.Write("DUTY").ReturnValue);
        }

        public double GetDerivativeGain(int channel)
        {
            return Convert.ToDouble(communicationsManager.Write("D").ReturnValue);
        }

        public double GetIntegralGain(int channel)
        {
            return Convert.ToDouble(communicationsManager.Write("I").ReturnValue);
        }

        public double GetPidLoopInterval()
        {
            return Convert.ToInt32(communicationsManager.Write("INTERVAL").ReturnValue);
        }

        public double GetProportionalGain(int channel)
        {
            return Convert.ToDouble(communicationsManager.Write("P").ReturnValue);
        }

        public void SetAngleSetPoint(int channel, double setPoint)
        {
            communicationsManager.Write(String.Format("SP {0}", setPoint));
        }

        public void SetDerivativeGain(int channel, double dGain)
        {
            communicationsManager.Write(String.Format("D {0}", dGain));
        }

        public void SetDutyCycle(int duty)
        {
            communicationsManager.Write(String.Format("DUTY {0}", duty));
        }

        public void SetIntegralGain(int channel, double iGain)
        {
            communicationsManager.Write(String.Format("I {0}", iGain));
        }

        public void SetPidLoopInterval(int interval)
        {
            communicationsManager.Write(String.Format("INTERVAL {0}", interval));
        }

        public void SetProportionalGain(double pGain)
        {
            communicationsManager.Write(String.Format("P {0}", pGain));
        }

        private void InitializeMicrocontroller()
        {
            DisablePid();
            DisableDebugMode();
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