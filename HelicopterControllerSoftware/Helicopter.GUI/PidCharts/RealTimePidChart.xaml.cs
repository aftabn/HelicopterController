// *************************************************************************************
// SCICHART® Copyright SciChart Ltd. 2011-2016. All rights reserved.
//
// Web: http://www.scichart.com
//   Support: support@scichart.com
//   Sales:   sales@scichart.com
//
// RealTimeCursors.xaml.cs is part of the SCICHART® Examples. Permission is hereby granted
// to modify, create derivative works, distribute and publish any part of this source
// code whether for commercial, private or personal use.
//
// The SCICHART® examples are distributed in the hope that they will be useful, but
// without any warranty. It is provided "AS IS" without warranty of any kind, either
// expressed or implied.
// *************************************************************************************

using Helicopter.Core.Database;
using log4net;
using SciChart.Charting.Model.DataSeries;
using System.ComponentModel;
using System.Linq;
using System.Windows.Controls;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for RealTimeChart.xaml
    /// </summary>
    public partial class RealTimePidChart : UserControl
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private const int FifoSampleSize = 100;
        private Session session;
        private ControllerDataSeries yawDataSeries;
        private ControllerDataSeries tiltDataSeries;

        private IXyDataSeries<double, double> yawAngles;
        private IXyDataSeries<double, double> yawSetPoints;
        private IXyDataSeries<double, double> tiltAngles;
        private IXyDataSeries<double, double> tiltSetPoints;

        public RealTimePidChart()
        {
            InitializeComponent();
        }

        public void StartNewSession(Session newSession)
        {
            session = newSession;
            yawDataSeries = session.YawDataSeries;
            tiltDataSeries = session.TiltDataSeries;

            session.PropertyChanged += OnNewControllerDataReceived;

            ClearDataSeries();

            yawAngles = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Yaw Angle" };
            yawSetPoints = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Yaw Set Point" };
            tiltAngles = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Tilt Angle" };
            tiltSetPoints = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Tilt Set Point" };

            YawAngleSeries.DataSeries = yawAngles;
            YawSetPointSeries.DataSeries = yawSetPoints;
            TiltAngleSeries.DataSeries = tiltAngles;
            TiltSetPointSeries.DataSeries = tiltSetPoints;

            Log.Debug("Starting PID Charting");
        }

        public void EndSession()
        {
            session.PropertyChanged -= OnNewControllerDataReceived;
            session = null;
            yawDataSeries = null;
            tiltDataSeries = null;

            Log.Debug("Stopping PID Charting");
        }

        private void ClearDataSeries()
        {
            using (SciChartSurface.SuspendUpdates())
            {
                if (yawAngles == null) return;

                yawAngles.Clear();
                yawSetPoints.Clear();
                tiltAngles.Clear();
                tiltSetPoints.Clear();
            }
        }

        private void OnNewControllerDataReceived(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "YawControllerData":
                    {
                        var startTime = session.StartTime;
                        var newDataPoint = yawDataSeries.ControllerData.Last();

                        var time = (newDataPoint.TimeStamp - startTime).TotalSeconds;
                        var angle = newDataPoint.CurrentAngle;
                        var setPoint = newDataPoint.SetPoint;

                        // Suspending updates ensures we only get one redraw after both series have been appended to
                        using (SciChartSurface.SuspendUpdates())
                        {
                            yawAngles.Append(time, angle);
                            yawSetPoints.Append(time, setPoint);
                        }
                    }
                    break;

                case "TiltControllerData":
                    {
                        var startTime = session.StartTime;
                        var newDataPoint = tiltDataSeries.ControllerData.Last();

                        var time = (newDataPoint.TimeStamp - startTime).TotalSeconds;
                        var angle = newDataPoint.CurrentAngle;
                        var setPoint = newDataPoint.SetPoint;

                        // Suspending updates ensures we only get one redraw after both series have been appended to
                        using (SciChartSurface.SuspendUpdates())
                        {
                            tiltAngles.Append(time, angle);
                            tiltSetPoints.Append(time, setPoint);
                        }
                    }
                    break;
            }
        }
    }
}