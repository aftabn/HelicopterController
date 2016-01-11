using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HelicopterController.Core
{
    public class ArduinoMicrocontroller : Microcontroller
    {
        public ArduinoMicrocontroller(bool isEngMode)
            : base(isEngMode)
        {
        }
    }
}