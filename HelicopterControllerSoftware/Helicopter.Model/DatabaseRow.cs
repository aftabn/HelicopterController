using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Model
{
    public class DatabaseRow
    {
        public int Id { get; set; }

        public DateTime StartTime { get; set; }

        public double YawCWProportionalGain { get; set; }
        public double YawCWIntegralGain { get; set; }
        public double YawCWDerivativeGain { get; set; }

        public double YawCCWProportionalGain { get; set; }
        public double YawCCWIntegralGain { get; set; }
        public double YawCCWDerivativeGain { get; set; }

        public double TiltCWProportionalGain { get; set; }
        public double TiltCWIntegralGain { get; set; }
        public double TiltCWDerivativeGain { get; set; }

        public string Comments { get; set; }
    }
}