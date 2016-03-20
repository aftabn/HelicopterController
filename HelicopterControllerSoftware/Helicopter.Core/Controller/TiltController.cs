using System.Collections.Generic;

namespace Helicopter.Core.Controller
{
    public class TiltController : AngleController
    {
        private const MotorType motorType = MotorType.Tilt;

        public TiltController() : base((int)motorType)
        {
            MotorType = motorType;

            PidProfiles = new Dictionary<DirectionProfile, PidProfile>
            {
                { DirectionProfile.CW, new PidProfile { DirectionProfile = DirectionProfile.CW } }
            };

            PidProfiles[DirectionProfile.CW].PropertyChanged += OnPidValuesPropertyChanged;
        }
    }
}