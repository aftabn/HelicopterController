using Helicopter.Core.Controller;
using Libs.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Settings
{
    public class TiltControllerSettings : AngleControllerSettings
    {
        public static TiltControllerSettings FromXmlElement(System.Xml.Linq.XElement angleControllerElement)
        {
            return new TiltControllerSettings()
            {
                IsEnabled = angleControllerElement.Attribute("IsEnabled").ParseOptionalBoolean(),
                MotorDriver = (MotorDriver)Enum.Parse(typeof(MotorDriver), angleControllerElement.Attribute("MotorDriver").Value),
                ProportionalGain = angleControllerElement.Attribute("ProportionalGain").ParseDouble(),
                IntegralGain = angleControllerElement.Attribute("IntegralGain").ParseDouble(),
                DerivativeGain = angleControllerElement.Attribute("DerivativeGain").ParseDouble(),
                IntegralWindupThreshold = angleControllerElement.Attribute("IntegralWindupThreshold").ParseDouble(),
                OutputRateLimit = angleControllerElement.Attribute("OutputRateLimit").ParseInt(),
            };
        }
    }
}