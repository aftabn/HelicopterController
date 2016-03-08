using Helicopter.Core.Sessions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for SessionPidChartWindow.xaml
    /// </summary>
    public partial class SessionPidChartWindow : Window
    {
        public SessionPidChartWindow(Session session)
        {
            InitializeComponent();

            UpdateGui(session);
            staticPidChart.LoadNewData(session);
        }

        private void UpdateGui(Session session)
        {
            var yaw = session.YawDataSeries;
            var tilt = session.TiltDataSeries;

            sessionStartTimeTextBlock.Text = session.StartTime.ToString();
            sessionEndTimeTextBlock.Text = session.EndTime.ToString();

            yawCWPGainTextBlock.Text = yaw.CWProportionalGain.ToString();
            yawCWIGainTextBlock.Text = yaw.CWIntegralGain.ToString();
            yawCWDGainTextBlock.Text = yaw.CWDerivativeGain.ToString();
            yawCCWPGainTextBlock.Text = yaw.CCWProportionalGain.ToString();
            yawCCWIGainTextBlock.Text = yaw.CCWIntegralGain.ToString();
            yawCCWDGainTextBlock.Text = yaw.CCWDerivativeGain.ToString();
            yawOutputRateLimitTextBlock.Text = yaw.OutputRateLimit.ToString();
            yawDriverType.Text = yaw.MotorDriver.ToString();

            tiltPGainTextBlock.Text = tilt.CWProportionalGain.ToString();
            tiltIGainTextBlock.Text = tilt.CWIntegralGain.ToString();
            tiltDGainTextBlock.Text = tilt.CWDerivativeGain.ToString();
            tiltOutputRateLimitTextBlock.Text = tilt.OutputRateLimit.ToString();
            tiltDriverType.Text = tilt.MotorDriver.ToString();

            sessionComment.Text = session.Comment;
        }
    }
}