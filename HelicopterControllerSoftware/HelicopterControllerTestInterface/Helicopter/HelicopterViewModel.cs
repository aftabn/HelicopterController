using Helicopter.Controller;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter
{
    public class HelicopterViewModel : INotifyPropertyChanged, IDisposable
    {
        private HelicopterController helicopterController;
        private bool isDeveloperMode;

        public HelicopterViewModel(bool isDeveloperMode)
        {
            this.isDeveloperMode = isDeveloperMode;
            helicopterController = new HelicopterController(isDeveloperMode);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void Dispose()
        {
        }
    }
}