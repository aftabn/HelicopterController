namespace Helicopter.PidGraph
{
    partial class PidGraph
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
            this.yawPidChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.tiltPidChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            ((System.ComponentModel.ISupportInitialize)(this.yawPidChart)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tiltPidChart)).BeginInit();
            this.SuspendLayout();
            // 
            // yawPidChart
            // 
            this.yawPidChart.Location = new System.Drawing.Point(12, 12);
            this.yawPidChart.Name = "yawPidChart";
            this.yawPidChart.Size = new System.Drawing.Size(762, 335);
            this.yawPidChart.TabIndex = 0;
            this.yawPidChart.Text = "chart1";
            title1.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Bold);
            title1.Name = "yawTitle";
            title1.Text = "Yaw";
            this.yawPidChart.Titles.Add(title1);
            // 
            // tiltPidChart
            // 
            this.tiltPidChart.Location = new System.Drawing.Point(12, 359);
            this.tiltPidChart.Name = "tiltPidChart";
            this.tiltPidChart.Size = new System.Drawing.Size(762, 335);
            this.tiltPidChart.TabIndex = 1;
            this.tiltPidChart.Text = "chart1";
            title2.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Bold);
            title2.Name = "tiltTitle";
            title2.Text = "Tilt";
            this.tiltPidChart.Titles.Add(title2);
            // 
            // PidGraph
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(787, 706);
            this.Controls.Add(this.tiltPidChart);
            this.Controls.Add(this.yawPidChart);
            this.Name = "PidGraph";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.yawPidChart)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tiltPidChart)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart yawPidChart;
        private System.Windows.Forms.DataVisualization.Charting.Chart tiltPidChart;
    }
}

