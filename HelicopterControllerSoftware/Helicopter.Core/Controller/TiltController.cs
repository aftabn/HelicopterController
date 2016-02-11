using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Controller
{
    public class TiltController : AngleController
    {
        private const MotorType motorType = MotorType.Tilt;

        public TiltController() : base((int)motorType)
        {
            MotorType = motorType;
        }
    }
}