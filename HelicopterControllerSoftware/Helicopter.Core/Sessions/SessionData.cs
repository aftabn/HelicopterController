using Helicopter.Core.Controller;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Helicopter.Core.Sessions
{
    public class SessionData
    {
        public SessionData(HelicopterController helicopterController)
        {
            YawControllerData = helicopterController.Yaw.ControllerData;
            TiltControllerData = helicopterController.Tilt.ControllerData;
        }

        public List<ControllerData> YawControllerData { get; set; }

        public List<ControllerData> TiltControllerData { get; set; }

        public List<DateTime> TimeStamps { get; set; }

        public IEnumerable<double> YawSetPoints
        {
            get
            {
                return YawControllerData.Select(x => x.SetPoint);
            }
        }

        public IEnumerable<double> YawAngles
        {
            get
            {
                return YawControllerData.Select(x => x.CurrentAngle);
            }
        }

        public IEnumerable<double> TiltSetPoints
        {
            get
            {
                return TiltControllerData.Select(x => x.SetPoint);
            }
        }

        public IEnumerable<double> TiltAngles
        {
            get
            {
                return TiltControllerData.Select(x => x.CurrentAngle);
            }
        }
    }
}