using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HelicopterController.Core
{
    public class VirtualMicrocontroller : Microcontroller
    {
        private const double DBL_ProportionalGain = 0.01;
        private const double DBL_IntegralGain = 0.0;
        private const double DBL_DerivativeGain = 0.0;
        private const double DBL_AngleSetPoint = 0;
        private const int INT_PidLoopInterval = 1;
        private const int INT_DutyCycle = 0;
        private const int INT_MaximumDutyCycle = 100;

        public VirtualMicrocontroller(bool isEngMode)
            : base(isEngMode)
        {
        }

        public override void RefreshProportionalGain()
        {
            ProportionalGain = DBL_ProportionalGain;
        }

        public override void RefreshIntegralGain()
        {
            IntegralGain = DBL_IntegralGain;
        }

        public override void RefreshDerivativeGain()
        {
            DerivativeGain = DBL_DerivativeGain;
        }

        public override void RefreshAngleSetPoint()
        {
            AngleSetPoint = DBL_AngleSetPoint;
        }

        public override void RefreshPidLoopInterval()
        {
            PidLoopInterval = INT_PidLoopInterval;
        }

        public override void RefreshDutyCycle()
        {
            DutyCycle = INT_DutyCycle;
        }

        public override void RefreshMaximumDutyCycle()
        {
            MaximumDutyCycle = INT_MaximumDutyCycle;
        }
    }
}