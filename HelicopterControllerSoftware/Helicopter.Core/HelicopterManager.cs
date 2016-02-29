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
        private const int INT_ThreadJoinTimeout = 3000;
        private HelicopterSettings helicopterSettings;
        private Thread sessionThread;
        private bool isSessionComplete;
        private bool isSessionAborted;

        public HelicopterManager()
        {
            HelicopterController = new HelicopterController();
            HelicopterController.PropertyChanged += OnControllerPropertyChanged;

            helicopterSettings = HelicopterSettings.Load();
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

        public bool IsPidSessionAlive
        {
            get
            {
                return (sessionThread != null ? sessionThread.IsAlive : false);
            }
        }

        public bool IsSessionAborted
        {
            get
            {
                return isSessionAborted;
            }

            set
            {
                if (value != isSessionAborted)
                {
                    isSessionAborted = value;
                    RaisePropertyChanged("IsSessionAborted");
                }
            }
        }

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

        public void Connect()
        {
            HelicopterController.Connect();
            HelicopterController.LoadSettings(helicopterSettings.ControllerSettings);
        }

        public void Disconnect()
        {
            StopPidSession();
            HelicopterController.Disconnect();
        }

        public void Dispose()
        {
        }

        public void StartPidSession()
        {
            if (IsPidSessionAlive)
            {
                throw new Exception("A PID session is currently ongoing. You can only start a new session once the previous one has finished.");
            }
            else if (HelicopterController.IsPidEnabled)
            {
                throw new Exception("PID is already enabled.");
            }
            else
            {
                StartSession();
                HelicopterController.EnablePid();
            }
        }

        public void StopPidSession()
        {
            if (IsPidSessionAlive)
            {
                HelicopterController.DisablePid();
                StopSession();
                Session.PropertyChanged -= OnSessionPropertyChanged;
            }
        }

        private void StartSession()
        {
            ResetSession();

            if (!IsPidSessionAlive)
            {
                Session.PropertyChanged += OnSessionPropertyChanged;
                sessionThread = new Thread(Session.Start);
                sessionThread.Name = "PidSessionThread";
                sessionThread.Start();

                while (!IsPidSessionAlive)
                {
                    Thread.Sleep(10);
                }
            }
            else
            {
                throw new Exception("A PID session is currently ongoing. You can only start a new session once the previous one has finished.");
            }
        }

        private void ResetSession()
        {
            isSessionComplete = false;
            isSessionAborted = false;
        }

        private void StopSession()
        {
            if (IsPidSessionAlive)
            {
                log.DebugFormat("Stopping routine thread");
                Session.Stop();
                CleanUpThreadResources();
            }
        }

        private void CleanUpThreadResources()
        {
            Session.PropertyChanged -= OnSessionPropertyChanged;
            if (sessionThread.Join(INT_ThreadJoinTimeout))
            {
                sessionThread = null;
                IsSessionComplete = true;
                log.DebugFormat("Routine thread has been stopped");
            }
            else
            {
                log.DebugFormat("Could not stop {0}", sessionThread.Name);
                KillRoutineThread();
                //throw new Exception("Could not kill session thread");
            }
        }

        private void KillRoutineThread()
        {
            log.DebugFormat("Forcibly killing routine thread");

            if (sessionThread != null && sessionThread.IsAlive)
            {
                sessionThread.Abort();
                //sessionThread = null;
                IsSessionAborted = true;
            }

            if (sessionThread.IsAlive)
            {
                throw new Exception("Could not kill session thread");
            }
            log.DebugFormat("Routine thread has been forcibly killed");
        }

        public void DisableMotors()
        {
            HelicopterController.DisableMotors();
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