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

using Helicopter.Core.Sessions;
using SciChart.Charting.Model.DataSeries;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for RealTimeChart.xaml
    /// </summary>
    public partial class RealTimePidChart : UserControl
    {
        private const int FifoSampleSize = 100;
        private ControllerDataSeries controllerDataSeries;

        private DateTime startTime;

        private IXyDataSeries<double, double> angles;
        private IXyDataSeries<double, double> setPoints;

        public RealTimePidChart()
        {
            InitializeComponent();
        }

        public void StartNewSession(ControllerDataSeries controllerDataSeries)
        {
            this.controllerDataSeries = controllerDataSeries;

            sciChartSurface.ChartTitle = controllerDataSeries.Name;

            controllerDataSeries.PropertyChanged += OnNewControllerData;

            ClearDataSeries();

            angles = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Current Angle" };
            setPoints = new XyDataSeries<double, double>() { FifoCapacity = FifoSampleSize, SeriesName = "Set Point" };

            angleSeries.DataSeries = angles;
            setPointSeries.DataSeries = setPoints;
        }

        public void EndSession()
        {
            controllerDataSeries.PropertyChanged -= OnNewControllerData;
            controllerDataSeries = null;
        }

        private void ClearDataSeries()
        {
            using (sciChartSurface.SuspendUpdates())
            {
                if (angles != null)
                {
                    angles.Clear();
                    setPoints.Clear();
                }
            }
        }

        private void OnNewControllerData(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ControllerData")
            {
                var startTime = controllerDataSeries.ControllerData.First().TimeStamp;
                var newDataPoint = controllerDataSeries.ControllerData.Last();

                double time = (newDataPoint.TimeStamp - startTime).TotalSeconds;
                double angle = newDataPoint.CurrentAngle;
                double setPoint = newDataPoint.SetPoint;

                // Suspending updates ensures we only get one redraw after both series have been appended to
                using (sciChartSurface.SuspendUpdates())
                {
                    angles.Append(time, angle);
                    setPoints.Append(time, setPoint);
                }
            }
        }
    }
}