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
        private bool isSessionRunning;
        private bool isSessionComplete;
        private bool isSessionRequestingStop;

        public HelicopterManager()
        {
            helicopterSettings = HelicopterSettings.Load();

            HelicopterController = new HelicopterController(helicopterSettings.ConnectionType);
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            InitializeSessionWorker();
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
                if (value != isSessionRunning)
                {
                    isSessionRunning = value;
                    RaisePropertyChanged("IsSessionRunning");
                }
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
                if (value != isSessionComplete)
                {
                    isSessionComplete = value;
                    RaisePropertyChanged("IsSessionComplete");
                }
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
                if (value != isSessionRequestingStop)
                {
                    isSessionRequestingStop = value;
                    RaisePropertyChanged("IsSessionRequestingStop");
                }
            }
        }

        public void Connect()
        {
            HelicopterController.Connect(helicopterSettings.ControllerSettings);
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

        public void StartSession()
        {
            if (IsSessionRunning)
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
            }
        }

        public void StopSession()
        {
            HelicopterController.DisablePid();
            if (sessionWorker.IsBusy)
            {
                sessionWorker.CancelAsync();
            }
        }

        public void StartDemo()
        {
            if (IsSessionRunning)
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
            Session = new Session(HelicopterController, helicopterSettings.PidThreadRefreshIntervalMilliseconds);
            Session.ClearControllerData();

            IsSessionComplete = false;
            IsSessionRunning = true;
        }

        private void SessionWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            var worker = sender as BackgroundWorker;

            HelicopterController.EnablePid();

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

        // TODO: Refactor this
        private void SessionWorker_DoDemoWork(object sender, DoWorkEventArgs e)
        {
            var worker = sender as BackgroundWorker;
            var yaw = HelicopterController.Yaw;
            var tilt = HelicopterController.Tilt;

            HelicopterController.ZeroYawAngle();
            yaw.SetSetPoint(0);
            tilt.SetSetPoint(18);

            Session.StartTime = DateTime.Now;

            HelicopterController.EnablePid();

            int steadyStateCount = 0;

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

                if (steadyStateCount >= 5)
                {
                    steadyStateCount = 0;
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

                if (steadyStateCount >= 5)
                {
                    steadyStateCount = 0;
                    break;
                }

                Thread.Sleep(Session.RefreshIntervalMilliseconds);
            }

            if (worker.CancellationPending)
            {
                Session.EndTime = DateTime.Now;
                e.Cancel = true;
            }

            tilt.SetSetPoint(-22);

            Session.EndTime = DateTime.Now;
            IsSessionRequestingStop = true;
        }

        private void SessionWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            IsSessionRunning = false;

            if (e.Error != null)
            {
                HelicopterController.DisablePid();
                throw new Exception(String.Format("Background worker stopped. {0}", e.Error.Message), e.Error.InnerException);
            }
            else
            {
                IsSessionComplete = true;
            }
        }

        //private void OnMaintainForceValueButton(object sender, EventArgs e)
        //{
        //    int count = 0;

        //    //keep track of packets
        //    try
        //    {
        //        //Intializing time values from GUI
        //        var holdTime = Convert.ToInt16(holdTimeTextbox.Text);
        //        var stretchTime = Convert.ToInt16(stretchTimeTextbox.Text);
        //        var startTime = DateTime.Now;
        //        double timeDifference;

        //        //Opening csv file for logging data and creating data column headers
        //        var csv = new StringBuilder();
        //        var filePath = @"C:\ProgramData\Kardium\DynamixelServoTestInterface\loadCellData.csv";
        //        if (!File.Exists(filePath))
        //        {
        //            File.Create(filePath).Close();
        //        }

        //        var columnHeaders = String.Format("Time (s),Force (N){0}", Environment.NewLine);
        //        csv.Append(columnHeaders);

        //        //starting stretch
        //        timeDifference = (DateTime.Now - startTime).TotalSeconds;
        //        while (timeDifference < holdTime)
        //        {
        //            timeDifference = (DateTime.Now - startTime).TotalSeconds;
        //            CloseForceGap();
        //            WriteLoadCellDataToCSVFile(csv, timeDifference);
        //            WaitInSeconds(0.1);
        //            count++;
        //        }

        //        UpdateOutputTextbox(String.Format("Done maintaining force for {0}s. Starting relax phase", holdTime));

        //        startTime = DateTime.Now;
        //        timeDifference = (DateTime.Now - startTime).TotalSeconds;
        //        while (timeDifference < stretchTime)
        //        {
        //            timeDifference = (DateTime.Now - startTime).TotalSeconds;
        //            GetAndDisplayForceControllerInfo();
        //            WriteLoadCellDataToCSVFile(csv, timeDifference);
        //            WaitInSeconds(0.1);
        //        }

        //        UpdateOutputTextbox("Done stretch test");

        //        File.WriteAllText(filePath, csv.ToString());
        //    }
        //    catch (Exception ex)
        //    {
        //        DisplayExceptionMessage(ex);
        //    }
        //    finally
        //    {
        //        UpdateOutputTextbox(String.Format("Close force gap called {0} times", count));
        //    }
        //}

        //private void WriteLoadCellDataToCSVFile(StringBuilder csv, double time)
        //{
        //    var force = Convert.ToDouble(getForceTextbox.Text);
        //    var temperature = dynamixelManager.Servos[currentServoId].Temperature;
        //    var newDataPoint = String.Format("{0:0.##},{1},{2}{3}", time, force, temperature, Environment.NewLine);
        //    csv.Append(newDataPoint);
        //}

        private void OnControllerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
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