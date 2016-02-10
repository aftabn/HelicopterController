using Helicopter.Core.Controller;
using Libs.Utilities;
using log4net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Settings
{
    public abstract class AngleControllerSettings
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public bool IsEnabled { get; protected set; }

        public MotorDriver MotorDriver { get; protected set; }

        public double ProportionalGain { get; protected set; }

        public double IntegralGain { get; protected set; }

        public double DerivativeGain { get; protected set; }

        public double IntegralWindupThreshold { get; protected set; }

        public int OutputRateLimit { get; protected set; }
    }
}