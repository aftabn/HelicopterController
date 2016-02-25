using Helicopter.Core.Controller;
using System;
using System.ComponentModel;
using System.Threading;

namespace Helicopter.Core.Sessions
{
    public class Session : INotifyPropertyChanged
    {
        private readonly object stopLock = new object();
        private bool stopping;
        private bool stopped;
        private HelicopterController helicopterController;
        private YawController yaw;
        private TiltController tilt;
        private int refreshIntervalMilliseconds;

        public Session(HelicopterController helicopterController, int refreshIntervalMilliseconds)
        {
            this.helicopterController = helicopterController;
            this.refreshIntervalMilliseconds = refreshIntervalMilliseconds;
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public SessionData SessionData { get; set; }

        public DateTime StartTime { get; private set; }

        public DateTime EndTime { get; private set; }

        public bool Stopping
        {
            get
            {
                lock (stopLock)
                {
                    return stopping;
                }
            }
        }

        public bool Stopped
        {
            get
            {
                lock (stopLock)
                {
                    return stopped;
                }
            }
        }

        public void Start()
        {
            SessionData = new SessionData(helicopterController);

            Reset();

            StartTime = DateTime.Now;

            while (!Stopping)
            {
                yaw.RefreshValues();
                tilt.RefreshValues();

                var timeStamp = DateTime.Now;
                SessionData.TimeStamps.Add(timeStamp);

                yaw.TakeNewDataSample(timeStamp);
                tilt.TakeNewDataSample(timeStamp);

                var time = DateTime.Now;
                while ((DateTime.Now - time).Milliseconds < refreshIntervalMilliseconds)
                {
                    if (Stopping)
                    {
                        break;
                    }
                    Thread.Sleep(10);
                }
            }

            EndTime = DateTime.Now;
            SetStopped();
        }

        public void Stop()
        {
            lock (stopLock)
            {
                stopping = true;
            }
        }

        private void SetStopped()
        {
            lock (stopLock)
            {
                stopped = true;
            }
        }

        private void Reset()
        {
            stopping = false;
            stopped = false;

            yaw.ControllerData.Clear();
            tilt.ControllerData.Clear();

            StartTime = DateTime.MinValue;
            EndTime = DateTime.MinValue;
        }

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}