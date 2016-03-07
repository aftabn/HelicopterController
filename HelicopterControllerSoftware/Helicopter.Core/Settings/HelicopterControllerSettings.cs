using Libs.Utilities;
using log4net;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Helicopter.Core.Settings
{
    public class HelicopterControllerSettings
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public YawControllerSettings YawControllerSettings { get; private set; }

        public TiltControllerSettings TiltControllerSettings { get; private set; }

        public bool IsSafetyEnabled { get; set; }

        public int PidLoopIntervalMilliseconds { get; set; }

        public static HelicopterControllerSettings FromXmlElement(XElement controllerElement)
        {
            var controllerSetting = new HelicopterControllerSettings()
            {
                IsSafetyEnabled = controllerElement.Attribute("IsSafetyEnabled").ParseOptionalBoolean(),
                PidLoopIntervalMilliseconds = controllerElement.Attribute("PidLoopIntervalMilliseconds").ParseInt(),
                YawControllerSettings = YawControllerSettings.FromXmlElement(controllerElement.Element("Yaw")),
                TiltControllerSettings = TiltControllerSettings.FromXmlElement(controllerElement.Element("Tilt")),
            };

            return controllerSetting;
        }
    }
}