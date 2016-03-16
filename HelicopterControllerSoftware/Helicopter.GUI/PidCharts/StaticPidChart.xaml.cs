using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Model;
using SciChart.Charting.Model.DataSeries;
using SciChart.Examples.ExternalDependencies.Common;
using SciChart.Examples.ExternalDependencies.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for StaticPidChart.xaml
    /// </summary>
    public partial class StaticPidChart : UserControl
    {
        private IXyDataSeries<double, double> yawAngles;
        private IXyDataSeries<double, double> yawSetPoints;
        private IXyDataSeries<double, double> tiltAngles;
        private IXyDataSeries<double, double> tiltSetPoints;

        public StaticPidChart()
        {
            yawAngles = new XyDataSeries<double, double>() { SeriesName = "Yaw Angle" };
            yawSetPoints = new XyDataSeries<double, double>() { SeriesName = "Yaw Set Point" };
            tiltAngles = new XyDataSeries<double, double>() { SeriesName = "Tilt Angle" };
            tiltSetPoints = new XyDataSeries<double, double>() { SeriesName = "Tilt Set Point" };

            InitializeComponent();
        }

        public void LoadNewData(SessionRecord sessionRecord)
        {
            var yaw = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Yaw.ToString());
            var tilt = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Tilt.ToString());

            var yawTimes = yaw.MeasurementRecords.Select(x => (x.TimeStamp - sessionRecord.StartTime).TotalSeconds);
            var yawAngles = yaw.MeasurementRecords.Select(x => x.CurrentAngle);
            var yawSetPoints = yaw.MeasurementRecords.Select(x => x.SetPoint);

            var tiltTimes = tilt.MeasurementRecords.Select(x => (x.TimeStamp - sessionRecord.StartTime).TotalSeconds);
            var tiltAngles = tilt.MeasurementRecords.Select(x => x.CurrentAngle);
            var tiltSetPoints = tilt.MeasurementRecords.Select(x => x.SetPoint);

            this.yawAngles.Append(yawTimes, yawAngles);
            this.yawSetPoints.Append(yawTimes, yawSetPoints);
            this.tiltAngles.Append(tiltTimes, tiltAngles);
            this.tiltSetPoints.Append(tiltTimes, tiltSetPoints);

            yawAngleSeries.DataSeries = this.yawAngles;
            yawSetPointSeries.DataSeries = this.yawSetPoints;
            tiltAngleSeries.DataSeries = this.tiltAngles;
            tiltSetPointSeries.DataSeries = this.tiltSetPoints;
        }
    }
}