using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Core.Settings;
using log4net;
using System;
using System.ComponentModel;
using System.Threading;

namespace Helicopter.Core
{
    public class HelicopterManager : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private HelicopterSettings helicopterSettings;
        private BackgroundWorker sessionWorker;
        private bool isPidSessionRunning;
        private bool isPidSessionComplete;

        public HelicopterManager()
        {
            HelicopterController = new HelicopterController();
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            helicopterSettings = HelicopterSettings.Load();

            InitializeSessionWorker();
            Session = new Session(HelicopterController, helicopterSettings.PidThreadRefreshIntervalMilliseconds);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public HelicopterController HelicopterController { get; private set; }

        public Session Session { get; private set; }

        public bool IsConnected
        {
            get
            {
                return HelicopterController.IsConnected;
            }
        }

        public bool IsPidSessionRunning
        {
            get
            {
                return isPidSessionRunning;
            }

            private set
            {
                if (value != isPidSessionRunning)
                {
                    isPidSessionRunning = value;
                    RaisePropertyChanged("IsPidSessionRunning");
                }
            }
        }

        public bool IsPidSessionComplete
        {
            get
            {
                return isPidSessionComplete;
            }

            private set
            {
                if (value != isPidSessionComplete)
                {
                    isPidSessionComplete = value;
                    RaisePropertyChanged("IsPidSessionComplete");
                }
            }
        }

        public void Connect()
        {
            HelicopterController.Connect();
            HelicopterController.LoadSettings(helicopterSettings.ControllerSettings);
        }

        public void Disconnect()
        {
            HelicopterController.Disconnect();
        }

        public void Dispose()
        {
        }

        public void DisableMotors()
        {
            HelicopterController.DisableMotors();
        }

        public void StartPidSession()
        {
            if (IsPidSessionRunning)
            {
                throw new Exception("A PID session is currently ongoing. You can only start a new session once the previous one has finished.");
            }
            else if (HelicopterController.IsPidEnabled)
            {
                throw new Exception("PID is already enabled.");
            }
            else
            {
                ResetSession();
                sessionWorker.RunWorkerAsync();
                HelicopterController.EnablePid();
            }
        }

        public void StopPidSession()
        {
            if (sessionWorker.IsBusy)
            {
                sessionWorker.CancelAsync();
            }
        }

        private void InitializeSessionWorker()
        {
            sessionWorker = new BackgroundWorker();
            sessionWorker.DoWork += new DoWorkEventHandler(SessionWorker_DoWork);
            sessionWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(SessionWorker_RunWorkerCompleted);
            sessionWorker.WorkerReportsProgress = false;
            sessionWorker.WorkerSupportsCancellation = true;
        }

        private void ResetSession()
        {
            Session.StartTime = DateTime.MinValue;
            Session.EndTime = DateTime.MinValue;

            Session.ClearControllerData();

            IsPidSessionComplete = false;
            IsPidSessionRunning = true;
        }

        private void SessionWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            var worker = sender as BackgroundWorker;

            Session.StartTime = DateTime.Now;

            while (!worker.CancellationPending)
            {
                var timeStamp = DateTime.Now;

                Session.TakeNewDataSamples(timeStamp);

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            Session.EndTime = DateTime.Now;

            e.Cancel = true;
        }

        private void SessionWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            IsPidSessionRunning = false;
            IsPidSessionComplete = true;
        }

        private void OnControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        private void OnSessionPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            var session = sender as Session;

            if (e.PropertyName == "StartTime")
            {
            }
            else if (e.PropertyName == "EndTime")
            {
            }
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