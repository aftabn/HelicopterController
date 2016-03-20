using Helicopter.Core.Controller;
using System;
using System.ComponentModel;

namespace Helicopter.Core.Database
{
    public class Session : INotifyPropertyChanged, IDisposable
    {
        private readonly YawController yaw;
        private readonly TiltController tilt;

        public Session(HelicopterController helicopterController, int refreshIntervalMilliseconds)
        {
            yaw = helicopterController.Yaw;
            tilt = helicopterController.Tilt;

            YawDataSeries = new ControllerDataSeries(yaw);
            TiltDataSeries = new ControllerDataSeries(tilt);

            RefreshIntervalMilliseconds = refreshIntervalMilliseconds;

            YawDataSeries.PropertyChanged += OnDataSeriesPropertyChanged;
            TiltDataSeries.PropertyChanged += OnDataSeriesPropertyChanged;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public int RefreshIntervalMilliseconds { get; private set; }

        public DateTime StartTime { get; set; }

        public DateTime EndTime { get; set; }

        public string Comment { get; set; }

        public ControllerDataSeries YawDataSeries { get; set; }

        public ControllerDataSeries TiltDataSeries { get; set; }

        public void Dispose()
        {
            YawDataSeries.PropertyChanged -= OnDataSeriesPropertyChanged;
            TiltDataSeries.PropertyChanged -= OnDataSeriesPropertyChanged;

            YawDataSeries = null;
            TiltDataSeries = null;
        }

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

        private void OnDataSeriesPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}