using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Sessions
{
    public class AngleControllerData
    {
        public AngleControllerData()
        {
            ControllerData = new List<ControllerData>();
        }

        public double ProportionalGain { get; set; }

        public double IntegralGain { get; set; }

        public double DerivativeGain { get; set; }

        public double IntegralWindupThreshold { get; set; }

        public int OutputRateLimit { get; set; }

        public List<ControllerData> ControllerData { get; set; }
    }
}