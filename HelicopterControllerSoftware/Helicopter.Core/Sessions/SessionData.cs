using Helicopter.Core.Controller;
using System.Collections.Generic;

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
    }
}