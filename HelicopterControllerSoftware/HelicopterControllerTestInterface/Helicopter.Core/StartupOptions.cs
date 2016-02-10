using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public class StartupOptions
    {
        public StartupOptions(String[] args)
        {
            IsDeveloperMode = args.Contains("-devmode", StringComparer.InvariantCultureIgnoreCase);
        }

        public StartupOptions()
        {
        }

        public bool IsDeveloperMode { get; set; }
    }
}