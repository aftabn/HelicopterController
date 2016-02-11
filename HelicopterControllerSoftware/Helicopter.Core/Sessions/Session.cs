using Helicopter.Core.Controller;
using System;
using System.ComponentModel;

namespace Helicopter.Core.Sessions
{
    public class Session : INotifyPropertyChanged
    {
        private readonly object stopLock = new object();
        private bool stopping;
        private bool stopped;
        private volatile bool isSessionComplete;
        private HelicopterController helicopterController;

        public Session()
        {
            SessionData = new SessionData();
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

        public void StartRoutine()
        {
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