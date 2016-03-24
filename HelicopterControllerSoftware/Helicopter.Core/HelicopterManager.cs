using Helicopter.Core.Controller;
using Helicopter.Core.Database;
using Helicopter.Core.Settings;
using log4net;
using System;
using System.ComponentModel;
using System.Threading;

namespace Helicopter.Core
{
    public class HelicopterManager : INotifyPropertyChanged, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private BackgroundWorker tuningWorker;
        private BackgroundWorker demoWorker;
        private bool isSessionRunning;
        private bool isSessionComplete;
        private bool isSessionRequestingStop;

        public HelicopterManager()
        {
            HelicopterSettings = HelicopterSettings.Load();

            HelicopterController = new HelicopterController(HelicopterSettings.ConnectionType);
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            InitializeTuningSessionWorker();
            InitializeDemoSessionWorker();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public HelicopterSettings HelicopterSettings { get; private set; }

        public HelicopterController HelicopterController { get; private set; }

        public Session Session { get; private set; }

        public bool IsConnected => HelicopterController.IsConnected;

        public ConnectionType ConnectionType
        {
            get { return HelicopterController.ConnectionType; }
            set { HelicopterController.ConnectionType = value; }
        }

        public bool IsSessionRunning
        {
            get
            {
                return isSessionRunning;
            }

            private set
            {
                if (value == isSessionRunning) return;

                isSessionRunning = value;
                RaisePropertyChanged("IsSessionRunning");
            }
        }

        public bool IsSessionComplete
        {
            get
            {
                return isSessionComplete;
            }

            private set
            {
                if (value == isSessionComplete) return;

                isSessionComplete = value;
                RaisePropertyChanged("IsSessionComplete");
            }
        }

        public bool IsSessionRequestingStop
        {
            get
            {
                return isSessionRequestingStop;
            }

            private set
            {
                if (value == isSessionRequestingStop) return;

                isSessionRequestingStop = value;
                RaisePropertyChanged("IsSessionRequestingStop");
            }
        }

        public void Connect()
        {
            HelicopterController.Connect(HelicopterSettings.ControllerSettings);
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

        public void StartTuningSession()
        {
            InitializeSession();
            tuningWorker.RunWorkerAsync();
        }

        public void StartDemoSession()
        {
            InitializeSession();
            demoWorker.RunWorkerAsync();
        }

        public void StopSession()
        {
            if (tuningWorker.IsBusy)
            {
                tuningWorker.CancelAsync();
            }
            else if (demoWorker.IsBusy)
            {
                demoWorker.CancelAsync();
            }
        }

        private void InitializeSession()
        {
            if (IsSessionRunning)
            {
                throw new Exception("A PID session is currently ongoing. You can only start a new session once the previous one has finished.");
            }
            else if (HelicopterController.IsPidEnabled)
            {
                throw new Exception("PID is already enabled.");
            }

            ResetSession();
            Log.Debug("Initialized new session");
        }

        private void InitializeTuningSessionWorker()
        {
            tuningWorker = new BackgroundWorker();
            tuningWorker.DoWork += SessionWorker_DoWork;
            tuningWorker.RunWorkerCompleted += SessionWorker_RunWorkerCompleted;
            tuningWorker.WorkerReportsProgress = false;
            tuningWorker.WorkerSupportsCancellation = true;
        }

        private void InitializeDemoSessionWorker()
        {
            demoWorker = new BackgroundWorker();
            demoWorker.DoWork += SessionWorker_DoDemoWork;
            demoWorker.RunWorkerCompleted += SessionWorker_RunWorkerCompleted;
            demoWorker.WorkerReportsProgress = false;
            demoWorker.WorkerSupportsCancellation = true;
        }

        private void ResetSession()
        {
            Session = new Session(HelicopterController, HelicopterSettings.PidThreadRefreshIntervalMilliseconds);
            Session.ClearControllerData();

            IsSessionComplete = false;
        }

        private void SessionWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            IsSessionRunning = true;

            Log.Debug("Started PID Tuning Session");

            var tilt = HelicopterController.Tilt;
            var worker = sender as BackgroundWorker;

            HelicopterController.EnablePid();

            Session.StartTime = DateTime.Now;

            while (!worker.CancellationPending)
            {
                var timeStamp = DateTime.Now;

                Session.TakeNewDataSamples(timeStamp);

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            HelicopterController.DisablePid();
            tilt.SetOutputPercentage(41);
            tilt.SetSetPoint(-21);

            var steadyStateCount = 0;
            var startDropTime = DateTime.Now;

            while (steadyStateCount < 2 && (DateTime.Now - startDropTime).TotalSeconds < 3)
            {
                Session.TakeNewDataSamples(DateTime.Now);

                if (Math.Abs(tilt.CurrentAngle - tilt.SetPoint) < 2)
                {
                    steadyStateCount++;
                }
                else
                {
                    steadyStateCount = 0;
                }

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            Session.EndTime = DateTime.Now;
            e.Cancel = true;
        }

        // TODO: Refactor this
        private void SessionWorker_DoDemoWork(object sender, DoWorkEventArgs e)
        {
            IsSessionRunning = true;

            Log.Debug("Started Demo Session");

            var worker = sender as BackgroundWorker;
            var yaw = HelicopterController.Yaw;
            var tilt = HelicopterController.Tilt;

            HelicopterController.ZeroYawAngle();
            yaw.SetSetPoint(0);
            tilt.SetSetPoint(18);

            Session.StartTime = DateTime.Now;

            HelicopterController.EnablePid();
            yaw.SetIntegralWindupThreshold(100);
            tilt.SetIntegralWindupThreshold(100);

            var steadyStateCount = 0;

            while (!worker.CancellationPending)
            {
                Session.TakeNewDataSamples(DateTime.Now);

                if (Math.Abs(tilt.CurrentAngle - tilt.SetPoint) < 2)
                {
                    steadyStateCount++;
                }
                else
                {
                    steadyStateCount = 0;
                }

                if (steadyStateCount >= 4)
                {
                    steadyStateCount = 0;
                    Log.Debug("Finished raising tilt motor");
                    break;
                }

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            if (worker.CancellationPending)
            {
                Session.EndTime = DateTime.Now;
                e.Cancel = true;
            }

            yaw.SetSetPoint(-180);

            while (!worker.CancellationPending)
            {
                var timeStamp = DateTime.Now;
                Session.TakeNewDataSamples(timeStamp);

                if (Math.Abs(yaw.CurrentAngle - yaw.SetPoint) < 2)
                {
                    steadyStateCount++;
                }
                else
                {
                    steadyStateCount = 0;
                }

                if (steadyStateCount >= 4)
                {
                    steadyStateCount = 0;
                    Log.Debug("Finished rotating yaw motor");
                    break;
                }

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            if (worker.CancellationPending)
            {
                Session.EndTime = DateTime.Now;
                e.Cancel = true;
            }

            HelicopterController.DisablePid();
            tilt.SetOutputPercentage(41);
            tilt.SetSetPoint(-21);

            while (!worker.CancellationPending)
            {
                var timeStamp = DateTime.Now;
                Session.TakeNewDataSamples(timeStamp);

                if (Math.Abs(tilt.CurrentAngle - tilt.SetPoint) < 2)
                {
                    steadyStateCount++;
                }
                else
                {
                    steadyStateCount = 0;
                }

                if (steadyStateCount >= 2)
                {
                    Log.Debug("Finished lowering tilt motor");
                    break;
                }

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            if (worker.CancellationPending)
            {
                Session.EndTime = DateTime.Now;
                e.Cancel = true;
            }

            Session.EndTime = DateTime.Now;
            IsSessionRequestingStop = true;
        }

        private void SessionWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            IsSessionRunning = false;
            HelicopterController.DisablePid();

            if (e.Error != null)
            {
                Log.DebugFormat("Background worker stopped. {0}", e.Error.Message);
                throw new Exception($"Background worker stopped. {e.Error.Message}", e.Error.InnerException);
            }

            IsSessionComplete = true;
            Log.Debug("Finished session");
        }

        private void OnControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}