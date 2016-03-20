using Helicopter.Core.Controller;
using Helicopter.Model;
using SciChart.Charting.Model.DataSeries;
using System.Linq;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for StaticPidChart.xaml
    /// </summary>
    public partial class StaticPidChart
    {
        private readonly IXyDataSeries<double, double> yawAngles;
        private readonly IXyDataSeries<double, double> yawSetPoints;
        private readonly IXyDataSeries<double, double> tiltAngles;
        private readonly IXyDataSeries<double, double> tiltSetPoints;

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

            YawAngleSeries.DataSeries = this.yawAngles;
            YawSetPointSeries.DataSeries = this.yawSetPoints;
            TiltAngleSeries.DataSeries = this.tiltAngles;
            TiltSetPointSeries.DataSeries = this.tiltSetPoints;
        }
    }
}