using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HelicopterController.Core
{
    public abstract class AngleController : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        protected CommunicationsManager microcontrollerCommunicationsManager;
        private bool isDeveloperMode;
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

        public AngleController(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;
        }

        public event PropertyChangedEventHandler PropertyChanged;

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

        public bool IsPidEnabled
        {
            get
            {
                return isPidEnabled;
            }
        }

        public void Dispose()
        {
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