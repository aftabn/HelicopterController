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

        public int PidLoopInterval { get; set; }

        public static HelicopterControllerSettings FromXmlElement(XElement controllerElement)
        {
            var controllerSetting = new HelicopterControllerSettings()
            {
                IsSafetyEnabled = controllerElement.Attribute("IsSafetyEnabled").ParseOptionalBoolean(),
                PidLoopInterval = controllerElement.Attribute("PidLoopInterval").ParseInt(),
                YawControllerSettings = YawControllerSettings.FromXmlElement(controllerElement.Element("Yaw")),
                TiltControllerSettings = TiltControllerSettings.FromXmlElement(controllerElement.Element("Tilt")),
            };

            return controllerSetting;
        }
    }
}