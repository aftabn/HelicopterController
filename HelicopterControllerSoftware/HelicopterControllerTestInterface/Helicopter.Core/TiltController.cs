using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public class TiltController : AngleController
    {
        private const int INT_TiltChannel = 1;

        public TiltController() : base(INT_TiltChannel)
        {
        }
    }
}