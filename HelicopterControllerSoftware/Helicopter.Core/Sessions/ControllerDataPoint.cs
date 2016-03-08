using System;

namespace Helicopter.Core.Sessions
{
    public class ControllerDataPoint
    {
        public ControllerDataPoint(DateTime timeStamp, double setPoint, double currentAngle)
        {
            TimeStamp = timeStamp;
            SetPoint = setPoint;
            CurrentAngle = currentAngle;
        }

        public DateTime TimeStamp { get; set; }

        public double SetPoint { get; set; }

        public double CurrentAngle { get; set; }
    }
}