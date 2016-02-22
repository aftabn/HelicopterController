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
        private volatile bool isSessionComplete;
        private HelicopterController helicopterController;
        private YawController yaw;
        private TiltController tilt;

        public Session(HelicopterController helicopterController)
        {
            this.helicopterController = helicopterController;
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;
            SessionData = new SessionData(helicopterController);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public event EventHandler<EventArgs> ErrorMessageReceived = delegate { };

        public SessionData SessionData { get; set; }

        public DateTime StartTime { get; private set; }

        public DateTime EndTime { get; private set; }

        public bool IsSessionComplete
        {
            get
            {
                return isSessionComplete;
            }

            set
            {
                if (value != isSessionComplete)
                {
                    isSessionComplete = value;
                    RaisePropertyChanged("IsSessionComplete");
                }
            }
        }

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

        public void StartSession()
        {
            yaw.ControllerData.Clear();
            tilt.ControllerData.Clear();

            StartTime = DateTime.Now;

            while (!stopping)
            {
                yaw.TakeNewDataSample();
                tilt.TakeNewDataSample();

                var time = DateTime.Now;
                while ((DateTime.Now - time).Milliseconds < 500)
                {
                    if (stopping)
                    {
                        break;
                    }
                    Thread.Sleep(10);
                }
            }

            EndTime = DateTime.Now;
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

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        //TODO: Can probably remove this later
        private void RaiseErrorMessageReceived(EventArgs e)
        {
            EventHandler<EventArgs> handler = ErrorMessageReceived;
            handler(this, e);
        }
    }
}