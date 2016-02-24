using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Helicopter.PidGraph
{
    public partial class PidGraph : Form
    {
        private Dictionary<MotorType, List<Series>> pidLineGraphs;
        private Dictionary<MotorType, List<Series>> setPointLineGraphs;
        private Dictionary<MotorType, ChartArea> chartAreas;

        public PidGraph()
        {
            InitializeComponent();
            //InitializeChartLayout();

            this.Text = "PID Position Graph";
        }

        public List<double> Angles { get; set; }

        public void SetDataBindings(SessionData sessionData)
        {
            var seriesList = new List<Series>();
        }

        private Series CreateAngleSeries(ControllerData controllerData)
        {
            var forceSeries = new Series
            {
                Name = "YawAngleSeries",
                Color = Color.Blue,
                XValueType = ChartValueType.Int32,
                ChartType = SeriesChartType.Line,
            };

            foreach (var measurement in controllerData.Measurements)
            {
                var zeroedTimeStamp = (measurement.TimeStamp - controllerData.StartTime).TotalSeconds;
                forceSeries.Points.AddXY(zeroedTimeStamp, measurement.Force);
            }

            return forceSeries;
        }

        private StripLine CreateAngleSetPointLine(ControllerData controllerData)
        {
            var setPoint = controllerData.SetPoint;

            var setPointLine = new StripLine
            {
                Text = String.Format("Set Point = {0}", setPoint),
                Interval = 0,
                IntervalOffset = setPoint,
                BorderColor = Color.Red,
                BorderDashStyle = ChartDashStyle.Dash
            };

            return setPointLine;
        }

        private void InitializeChartLayout()
        {
            chartAreas = new Dictionary<MotorType, ChartArea>();

            chartAreas[MotorType.Yaw] = new ChartArea();
            chartAreas[MotorType.Tilt] = new ChartArea();

            chartAreas[MotorType.Yaw].Name = "YawChartArea";
            chartAreas[MotorType.Yaw].AxisX.Title = "Time (s)";
            chartAreas[MotorType.Yaw].AxisX.Minimum = 0;
            chartAreas[MotorType.Yaw].AxisX.MajorGrid.LineColor = Color.LightGray;
            chartAreas[MotorType.Yaw].AxisY.Title = "Angle (deg)";
            chartAreas[MotorType.Yaw].AxisY.Minimum = -180;
            chartAreas[MotorType.Yaw].AxisY.Maximum = 180;
            chartAreas[MotorType.Yaw].AxisY.MajorGrid.LineColor = Color.LightGray;

            chartAreas[MotorType.Tilt].Name = "TiltChartArea";
            chartAreas[MotorType.Tilt].AxisX.Title = "Time (s)";
            chartAreas[MotorType.Tilt].AxisX.Minimum = 0;
            chartAreas[MotorType.Tilt].AxisX.MajorGrid.LineColor = Color.LightGray;
            chartAreas[MotorType.Tilt].AxisY.Title = "Angle (deg)";
            chartAreas[MotorType.Tilt].AxisY.Minimum = -145;
            chartAreas[MotorType.Tilt].AxisY.Maximum = 145;
            chartAreas[MotorType.Tilt].AxisY.MajorGrid.LineColor = Color.LightGray;

            var yawTitle = new Title
            {
                Name = "YawTitle",
                Text = "Yaw"
            };

            var tiltTitle = new Title
            {
                Name = "TiltTitle",
                Text = "Tilt"
            };

            yawPidChart.Titles.Add(yawTitle);
            yawPidChart.ChartAreas.Add(chartAreas[MotorType.Yaw]);

            tiltPidChart.Titles.Add(tiltTitle);
            tiltPidChart.ChartAreas.Add(chartAreas[MotorType.Tilt]);
        }
    }
}