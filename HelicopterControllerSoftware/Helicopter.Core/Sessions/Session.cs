using Helicopter.Core.Controller;
using System;
using System.ComponentModel;
using System.Threading;

namespace Helicopter.Core.Sessions
{
    public class Session
    {
        private YawController yaw;
        private TiltController tilt;

        public Session(HelicopterController helicopterController, int refreshIntervalMilliseconds)
        {
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;

            YawDataSeries = new ControllerDataSeries(yaw);
            TiltDataSeries = new ControllerDataSeries(tilt);

            RefreshIntervalMilliseconds = refreshIntervalMilliseconds;
        }

        public int RefreshIntervalMilliseconds { get; private set; }

        public DateTime StartTime { get; set; }

        public DateTime EndTime { get; set; }

        public ControllerDataSeries YawDataSeries { get; set; }

        public ControllerDataSeries TiltDataSeries { get; set; }

        public void TakeNewDataSamples(DateTime timeStamp)
        {
            yaw.TakeNewDataSample(timeStamp);
            tilt.TakeNewDataSample(timeStamp);
        }

        public void ClearControllerData()
        {
            yaw.ControllerData.Clear();
            tilt.ControllerData.Clear();
        }
    }
}