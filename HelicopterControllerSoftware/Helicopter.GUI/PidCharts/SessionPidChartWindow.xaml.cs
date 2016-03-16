using Helicopter.Core;
using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Model;
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
        private readonly SessionRecord sessionRecord;

        public SessionPidChartWindow(SessionRecord sessionRecord)
        {
            this.sessionRecord = sessionRecord;

            InitializeComponent();

            UpdateGui(sessionRecord);
            staticPidChart.LoadNewData(sessionRecord);

            sessionComment.Focus();
            sessionComment.CaretIndex = sessionComment.Text.Length;
        }

        private void UpdateGui(SessionRecord sessionRecord)
        {
            using (var context = new HelicopterModelEntities())
            {
                var yaw = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Yaw.ToString());
                var tilt = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Tilt.ToString());

                sessionStartTimeTextBlock.Text = sessionRecord.StartTime.ToString();
                sessionEndTimeTextBlock.Text = sessionRecord.EndTime.ToString();

                yawCWPGainTextBlock.Text = yaw.CWProportionalGain.ToString();
                yawCWIGainTextBlock.Text = yaw.CWIntegralGain.ToString();
                yawCWDGainTextBlock.Text = yaw.CWDerivativeGain.ToString();
                yawCCWPGainTextBlock.Text = yaw.CCWProportionalGain.ToString();
                yawCCWIGainTextBlock.Text = yaw.CCWIntegralGain.ToString();
                yawCCWDGainTextBlock.Text = yaw.CCWDerivativeGain.ToString();
                yawWindupTextBlock.Text = yaw.IntegralWindupThreshold.ToString();
                yawOutputRateLimitTextBlock.Text = yaw.OutputRateLimit.ToString();
                yawDriverType.Text = yaw.DriverType;

                tiltPGainTextBlock.Text = tilt.CWProportionalGain.ToString();
                tiltIGainTextBlock.Text = tilt.CWIntegralGain.ToString();
                tiltDGainTextBlock.Text = tilt.CWDerivativeGain.ToString();
                tiltWindupTextBlock.Text = yaw.IntegralWindupThreshold.ToString();
                tiltOutputRateLimitTextBlock.Text = tilt.OutputRateLimit.ToString();
                tiltDriverType.Text = tilt.DriverType;

                sessionComment.Text = sessionRecord.Comment;
            }
        }

        private void OnSaveCommentButtonClick(object sender, KeyEventArgs key)
        {
            if (key.Key == Key.Enter)
            {
                DatabaseManager.UpdateSessionComment(sessionRecord.Id, sessionComment.Text);
            }
        }
    }
}