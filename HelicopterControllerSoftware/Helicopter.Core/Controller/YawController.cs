using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core.Controller
{
    public class YawController : AngleController
    {
        private const MotorType motorType = MotorType.Yaw;

        public YawController() : base((int)motorType)
        {
            MotorType = motorType;

            PidProfiles = new Dictionary<DirectionProfile, PidProfile>
            {
                { DirectionProfile.CW, new PidProfile { DirectionProfile = DirectionProfile.CW } },
                { DirectionProfile.CCW, new PidProfile{ DirectionProfile = DirectionProfile.CCW } }
            };

            PidProfiles[DirectionProfile.CW].PropertyChanged += OnPidValuesPropertyChanged;
            PidProfiles[DirectionProfile.CCW].PropertyChanged += OnPidValuesPropertyChanged;
        }

        public void ZeroAngle()
        {
            Microcontroller.ZeroEncoderAngle();
            RefreshCurrentAngle();
        }
    }
}