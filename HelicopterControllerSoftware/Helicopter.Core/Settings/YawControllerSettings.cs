using Helicopter.Core.Controller;
using Libs.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Settings
{
    public class YawControllerSettings : AngleControllerSettings
    {
        public static YawControllerSettings FromXmlElement(System.Xml.Linq.XElement angleControllerElement)
        {
            var cwProfileElement = angleControllerElement.Elements("PidProfile").Single(x => x.Attribute("Direction").Value == DirectionProfile.CW.ToString());
            var ccwProfileElement = angleControllerElement.Elements("PidProfile").Single(x => x.Attribute("Direction").Value == DirectionProfile.CCW.ToString());

            var settings = new YawControllerSettings()
            {
                IsEnabled = angleControllerElement.Attribute("IsEnabled").ParseOptionalBoolean(),
                MotorDriver = (MotorDriver)Enum.Parse(typeof(MotorDriver), angleControllerElement.Attribute("MotorDriver").Value),
                IntegralWindupThreshold = angleControllerElement.Attribute("IntegralWindupThreshold").ParseDouble(),
                OutputRateLimit = angleControllerElement.Attribute("OutputRateLimit").ParseInt(),
                PidProfiles = new Dictionary<DirectionProfile, PidProfile>
                {
                    {DirectionProfile.CW, PidProfile.FromXmlElement(cwProfileElement) },
                    {DirectionProfile.CCW, PidProfile.FromXmlElement(ccwProfileElement) }
                },
            };

            return settings;
        }
    }
}