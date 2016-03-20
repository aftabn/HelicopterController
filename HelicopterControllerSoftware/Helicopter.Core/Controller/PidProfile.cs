using Libs.Utilities;
using System;
using System.ComponentModel;

namespace Helicopter.Core.Controller
{
    public class PidProfile : INotifyPropertyChanged
    {
        private double proportionalGain;
        private double integralGain;
        private double derivativeGain;

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

        public DirectionProfile DirectionProfile { get; set; }

        public double ProportionalGain

        {
            get
            {
                return proportionalGain;
            }

            set
            {
                if (value == proportionalGain) return;

                proportionalGain = value;
                RaisePropertyChanged("ProportionalGain");
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
                if (value == integralGain) return;

                integralGain = value;
                RaisePropertyChanged("IntegralGain");
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
                if (value == derivativeGain) return;

                derivativeGain = value;
                RaisePropertyChanged("DerivativeGain");
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs($"{DirectionProfile}{propertyName}"));
        }
    }
}