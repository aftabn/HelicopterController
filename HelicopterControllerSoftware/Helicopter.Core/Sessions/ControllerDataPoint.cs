using System;

namespace Helicopter.Core.Sessions
{
    public class ControllerDataPoint
    {
        public ControllerDataPoint()
        {
        }

        public DateTime TimeStamp { get; set; }

        public double SetPoint { get; set; }

        public double CurrentAngle { get; set; }

        public double ProportionalGain { get; set; }

        public double IntegralGain { get; set; }

        public double DerivativeGain { get; set; }

        public double IntegralWindupThreshold { get; set; }

        public int OutputRateLimit { get; set; }
    }
}