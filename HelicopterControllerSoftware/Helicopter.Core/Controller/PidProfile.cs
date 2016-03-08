using Libs.Utilities;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Controller
{
    public class PidProfile : INotifyPropertyChanged
    {
        private double proportionalGain;
        private double integralGain;
        private double derivativeGain;
        private DirectionProfile directionProfile;

        public PidProfile()
        {
        }

        public static PidProfile FromXmlElement(System.Xml.Linq.XElement pidProfileElement)
        {
            var profile = new PidProfile()
            {
                DirectionProfile = (DirectionProfile)Enum.Parse(typeof(DirectionProfile), pidProfileElement.Attribute("Direction").Value),
                ProportionalGain = pidProfileElement.Attribute("ProportionalGain").ParseDouble(),
                IntegralGain = pidProfileElement.Attribute("IntegralGain").ParseDouble(),
                DerivativeGain = pidProfileElement.Attribute("DerivativeGain").ParseDouble(),
            };

            return profile;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public DirectionProfile DirectionProfile
        {
            get
            {
                return directionProfile;
            }

            set
            {
                directionProfile = value;
            }
        }

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

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(String.Format("{0}{1}", DirectionProfile, propertyName)));
            }
        }
    }
}