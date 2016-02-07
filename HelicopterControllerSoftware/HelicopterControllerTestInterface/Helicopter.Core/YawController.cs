using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public class YawController : AngleController
    {
        private const int INT_YawChannel = 0;

        public YawController() : base(INT_YawChannel)
        {
        }
    }
}