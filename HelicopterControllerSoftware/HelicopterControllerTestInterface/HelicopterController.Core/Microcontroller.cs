using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HelicopterController.Core
{
    public abstract class Microcontroller : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        protected MicrocontrollerCommunicationsManager microcontrollerCommunicationsManager;
        private bool isEngMode;
        private bool isConnected;
        protected bool isDebugEnabled;
        protected bool isPidEnabled;
        private double proportionalGain;
        private double integralGain;
        private double derivativeGain;
        private double angleSetPoint;
        private int pidLoopInterval;
        private int dutyCycle;
        private int maximumDutyCycle;

        public Microcontroller(bool isEngMode)
        {
            this.isEngMode = isEngMode;
            microcontrollerCommunicationsManager = new MicrocontrollerCommunicationsManager(isEngMode);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsVirtual
        {
            get
            {
                return microcontrollerCommunicationsManager != null ? microcontrollerCommunicationsManager.IsVirtual : false;
            }
        }

        public bool IsConnected
        {
            get
            {
                return microcontrollerCommunicationsManager != null ? microcontrollerCommunicationsManager.IsConnected : false;
            }
        }

        public bool IsEngineeringMode { get { return isEngMode; } }

        public ControllerState ControllerState { get; set; }

        public double ProportionalGain
        {
            get
            {
                return proportionalGain;
            }

            set
            {
                if (value != proportionalGain)
                {
                    proportionalGain = value;
                    RaisePropertyChanged("ProportionalGain");
                }
            }
        }

        public double IntegralGain
        {
            get
            {
                return integralGain;
            }

            set
            {
                if (value != integralGain)
                {
                    integralGain = value;
                    RaisePropertyChanged("IntegralGain");
                }
            }
        }

        public double DerivativeGain
        {
            get
            {
                return derivativeGain;
            }

            set
            {
                if (value != derivativeGain)
                {
                    derivativeGain = value;
                    RaisePropertyChanged("DerivativeGain");
                }
            }
        }

        public double AngleSetPoint
        {
            get
            {
                return derivativeGain;
            }

            set
            {
                if (value != derivativeGain)
                {
                    derivativeGain = value;
                    RaisePropertyChanged("DerivativeGain");
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

        public int DutyCycle
        {
            get
            {
                return dutyCycle;
            }

            set
            {
                if (value != dutyCycle)
                {
                    dutyCycle = value;
                    RaisePropertyChanged("DutyCycle");
                }
            }
        }

        public int MaximumDutyCycle
        {
            get
            {
                return maximumDutyCycle;
            }

            set
            {
                if (value != maximumDutyCycle)
                {
                    maximumDutyCycle = value;
                    RaisePropertyChanged("MaximumDutyCycle");
                }
            }
        }

        public bool IsDebugEnabled
        {
            get
            {
                return isDebugEnabled;
            }
        }

        public bool IsPidEnabled
        {
            get
            {
                return isPidEnabled;
            }
        }

        public void Connect()
        {
            if (!IsConnected)
            {
                microcontrollerCommunicationsManager.Connect();
                InitializeMicrocontroller();
            }
            else
            {
                throw new Exception("Microcontroller is already connected.");
            }
        }

        private void InitializeMicrocontroller()
        {
            DisablePid();
            DisableDebugMode();
            RefreshProportionalGain();
            RefreshIntegralGain();
            RefreshDerivativeGain();
            RefreshAngleSetPoint();
            RefreshPidLoopInterval();
            RefreshDutyCycle();
            RefreshMaximumDutyCycle();
        }

        public void Dispose()
        {
            microcontrollerCommunicationsManager.Disconnect();
        }

        public virtual void RefreshProportionalGain()
        {
            ProportionalGain = Convert.ToDouble(microcontrollerCommunicationsManager.Write("P").ReturnValue);
        }

        public virtual void RefreshIntegralGain()
        {
            IntegralGain = Convert.ToDouble(microcontrollerCommunicationsManager.Write("I").ReturnValue);
        }

        public virtual void RefreshDerivativeGain()
        {
            DerivativeGain = Convert.ToDouble(microcontrollerCommunicationsManager.Write("D").ReturnValue);
        }

        public virtual void RefreshAngleSetPoint()
        {
            AngleSetPoint = Convert.ToDouble(microcontrollerCommunicationsManager.Write("SP").ReturnValue);
        }

        public virtual void RefreshPidLoopInterval()
        {
            PidLoopInterval = Convert.ToInt32(microcontrollerCommunicationsManager.Write("INTERVAL").ReturnValue);
        }

        public virtual void RefreshDutyCycle()
        {
            DutyCycle = Convert.ToInt32(microcontrollerCommunicationsManager.Write("DUTY").ReturnValue);
        }

        public virtual void RefreshMaximumDutyCycle()
        {
            MaximumDutyCycle = Convert.ToInt32(microcontrollerCommunicationsManager.Write("MAXDUTY").ReturnValue);
        }

        public void SetProportionalGain(double pGain)
        {
            microcontrollerCommunicationsManager.Write(String.Format("P {0}", pGain));
            ProportionalGain = pGain;
        }

        public void SetIntegralGain(double iGain)
        {
            microcontrollerCommunicationsManager.Write(String.Format("I {0}", iGain));
            IntegralGain = iGain;
        }

        public void SetDerivativeGain(double dGain)
        {
            microcontrollerCommunicationsManager.Write(String.Format("D {0}", dGain));
            DerivativeGain = dGain;
        }

        public void SetAngleSetPoint(double setPoint)
        {
            microcontrollerCommunicationsManager.Write(String.Format("SP {0}", setPoint));
            AngleSetPoint = setPoint;
        }

        public void SetPidLoopInterval(int interval)
        {
            microcontrollerCommunicationsManager.Write(String.Format("INTERVAL {0}", interval));
            PidLoopInterval = interval;
        }

        public void SetDutyCycle(int duty)
        {
            microcontrollerCommunicationsManager.Write(String.Format("DUTY {0}", duty));
            DutyCycle = duty;
        }

        public void SetMaxmimumDutyCycle(int maxDuty)
        {
            microcontrollerCommunicationsManager.Write(String.Format("MAXDUTY {0}", maxDuty));
            MaximumDutyCycle = maxDuty;
        }

        public void EnableDebugMode()
        {
            microcontrollerCommunicationsManager.Write("DEBUG ON");
            isDebugEnabled = true;
            RaisePropertyChanged("IsDebugEnabled");
        }

        public void DisableDebugMode()
        {
            microcontrollerCommunicationsManager.Write("DEBUG OFF");
            isDebugEnabled = false;
            RaisePropertyChanged("IsDebugEnabled");
        }

        public void EnablePid()
        {
            microcontrollerCommunicationsManager.Write("PID ON");
            isPidEnabled = true;
            RaisePropertyChanged("IsPidEnabled");
        }

        public void DisablePid()
        {
            microcontrollerCommunicationsManager.Write("PID OFF");
            isPidEnabled = false;
            RaisePropertyChanged("IsPidEnabled");
        }

        protected void OnComponentPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        protected void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}