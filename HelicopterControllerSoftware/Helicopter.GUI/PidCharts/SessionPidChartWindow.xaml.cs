using Helicopter.Core;
using Helicopter.Core.Controller;
using Helicopter.Model;
using System.Linq;
using System.Windows.Input;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for SessionPidChartWindow.xaml
    /// </summary>
    public partial class SessionPidChartWindow
    {
        private readonly SessionRecord sessionRecord;

        public SessionPidChartWindow(SessionRecord sessionRecord)
        {
            this.sessionRecord = sessionRecord;

            InitializeComponent();

            UpdateGui(sessionRecord);
            StaticPidChart.LoadNewData(sessionRecord);

            SessionComment.Focus();
            SessionComment.CaretIndex = SessionComment.Text.Length;
        }

        private void UpdateGui(SessionRecord sessionRecord)
        {
            using (var context = new HelicopterModelEntities())
            {
                var yaw = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Yaw.ToString());
                var tilt = sessionRecord.ControllerRecords.Single(x => x.MotorType == MotorType.Tilt.ToString());

                SessionStartTimeTextBlock.Text = sessionRecord.StartTime.ToString();
                SessionEndTimeTextBlock.Text = sessionRecord.EndTime.ToString();

                YawCWPGainTextBlock.Text = yaw.CWProportionalGain.ToString();
                YawCWIGainTextBlock.Text = yaw.CWIntegralGain.ToString();
                YawCWDGainTextBlock.Text = yaw.CWDerivativeGain.ToString();
                YawCCWPGainTextBlock.Text = yaw.CCWProportionalGain.ToString();
                YawCCWIGainTextBlock.Text = yaw.CCWIntegralGain.ToString();
                YawCCWDGainTextBlock.Text = yaw.CCWDerivativeGain.ToString();
                YawWindupTextBlock.Text = yaw.IntegralWindupThreshold.ToString();
                YawOutputRateLimitTextBlock.Text = yaw.OutputRateLimit.ToString();
                YawDriverType.Text = yaw.DriverType;

                TiltPGainTextBlock.Text = tilt.CWProportionalGain.ToString();
                TiltIGainTextBlock.Text = tilt.CWIntegralGain.ToString();
                TiltDGainTextBlock.Text = tilt.CWDerivativeGain.ToString();
                TiltWindupTextBlock.Text = yaw.IntegralWindupThreshold.ToString();
                TiltOutputRateLimitTextBlock.Text = tilt.OutputRateLimit.ToString();
                TiltDriverType.Text = tilt.DriverType;

                SessionComment.Text = sessionRecord.Comment;
            }
        }

        private void OnSaveCommentButtonClick(object sender, KeyEventArgs key)
        {
            if (key.Key == Key.Enter)
            {
                DatabaseManager.UpdateSessionComment(sessionRecord.Id, SessionComment.Text);
            }
        }
    }
}