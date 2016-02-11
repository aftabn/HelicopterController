using Helicopter.Core.Controller;
using System.Collections.Generic;

namespace Helicopter.Core.Sessions
{
    public class SessionData
    {
        public SessionData()
        {
            YawControllerData = new AngleControllerData();
            TiltControllerData = new AngleControllerData();
        }

        public AngleControllerData YawControllerData { get; set; }

        public AngleControllerData TiltControllerData { get; set; }
    }
}