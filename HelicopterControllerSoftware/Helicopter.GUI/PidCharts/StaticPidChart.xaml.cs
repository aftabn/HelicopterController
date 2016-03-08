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
            yawSetPoints = new XyDataSeries<double, double>() { SeriesName = " Yaw Set Point" };
            tiltAngles = new XyDataSeries<double, double>() { SeriesName = "Tilt Angle" };
            tiltSetPoints = new XyDataSeries<double, double>() { SeriesName = "Tilt Set Point" };

            InitializeComponent();
        }

        public void LoadNewData(Session session)
        {
            var yawTimes = session.YawDataSeries.ControllerData.Select(x => (x.TimeStamp - session.StartTime).TotalSeconds);
            var yawAngles = session.YawDataSeries.ControllerData.Select(x => x.CurrentAngle);
            var yawSetPoints = session.YawDataSeries.ControllerData.Select(x => x.SetPoint);

            var tiltTimes = session.YawDataSeries.ControllerData.Select(x => (x.TimeStamp - session.StartTime).TotalSeconds);
            var tiltAngles = session.YawDataSeries.ControllerData.Select(x => x.CurrentAngle);
            var tiltSetPoints = session.YawDataSeries.ControllerData.Select(x => x.SetPoint);

            this.yawAngles.Append(yawTimes, yawAngles);
            this.yawSetPoints.Append(yawTimes, yawSetPoints);
            this.tiltAngles.Append(tiltTimes, tiltAngles);
            this.tiltSetPoints.Append(tiltTimes, tiltSetPoints);

            yawAngleSeries.DataSeries = this.yawAngles;
            yawSetPointSeries.DataSeries = this.yawSetPoints;
            tiltAngleSeries.DataSeries = this.tiltAngles;
            tiltSetPointSeries.DataSeries = this.tiltSetPoints;
        }

        public void LoadNewData(SessionRecord sessionRecord)
        {
        }
    }
}