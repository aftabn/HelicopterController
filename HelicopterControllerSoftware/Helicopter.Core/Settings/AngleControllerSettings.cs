using Helicopter.Core.Controller;
using log4net;
using System.Collections.Generic;

namespace Helicopter.Core.Settings
{
    public abstract class AngleControllerSettings
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public bool IsEnabled { get; protected set; }

        public MotorDriver MotorDriver { get; protected set; }

        public Dictionary<DirectionProfile, PidProfile> PidProfiles { get; protected set; }

        public double IntegralWindupThreshold { get; protected set; }

        public int OutputRateLimit { get; protected set; }
    }
}