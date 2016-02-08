using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Controller
{
    public class YawController : AngleController
    {
        private const MotorType motorType = MotorType.Yaw;

        public YawController() : base((int)motorType)
        {
            MotorType = motorType;
        }
    }
}