﻿using Helicopter.Core;
using Helicopter.Core.Sessions;
using LiveCharts;
using log4net;
using System;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Media;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class HelicopterControllerWindow : Window, INotifyPropertyChanged
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private const int INT_NumSamplesPerWindow = 100;
        private readonly StartupOptions startupOptions;
        private HelicopterViewModel helicopterViewModel;

        public HelicopterControllerWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            helicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);
            helicopterViewModel.PropertyChanged += OnViewModelPropertyChanged;

            InitializeComponent();
            InitializePidCharts();
            SetBindingForControls();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public SeriesCollection YawSeries
        {
            get { return helicopterViewModel.YawSeries; }
            set { helicopterViewModel.YawSeries = value; }
        }

        public SeriesCollection TiltSeries
        {
            get { return helicopterViewModel.TiltSeries; }
            set { helicopterViewModel.TiltSeries = value; }
        }

        public Func<double, string> XFormatter
        {
            get { return helicopterViewModel.XFormatter; }
            set { helicopterViewModel.XFormatter = value; }
        }

        public Func<double, string> YFormatter
        {
            get { return helicopterViewModel.YFormatter; }
            set { helicopterViewModel.YFormatter = value; }
        }

        private void UpdateStatusBar()
        {
            SolidColorBrush statusBarColor;

            if (helicopterViewModel.IsConnected)
            {
                helicopterControllerStatusTextbox.Text = "Helicopter Controller Connected";
                statusBarColor = new SolidColorBrush(Color.FromRgb(0x00, 0x7A, 0xCC)); // Blue
            }
            else
            {
                helicopterControllerStatusTextbox.Text = "Helicopter Controller Not Connected";
                statusBarColor = Brushes.Firebrick;
            }

            statusBar.Background = statusBarColor;
        }

        private void OnWindowClosing(object sender, CancelEventArgs e)
        {
            switch (MessageBox.Show("Are you sure you want to close?", "Close Confirmation", MessageBoxButton.YesNo))
            {
                case MessageBoxResult.No:
                    e.Cancel = true;
                    break;

                default:
                    log.DebugFormat("Closing the application from messagebox");

                    if (helicopterViewModel.IsConnected)
                    {
                        helicopterViewModel.Disconnect();
                    }

                    break;
            }
        }

        private void SetBindingForControls()
        {
            menuHeader.DataContext = helicopterViewModel;
            optionsToolbar.DataContext = helicopterViewModel;
            controllerTabs.DataContext = helicopterViewModel;
            controllerOutputPanel.DataContext = helicopterViewModel;
        }

        private void InitializePidCharts()
        {
            var yawConfig = new SeriesConfiguration<ControllerData>();
            var tiltConfig = new SeriesConfiguration<ControllerData>();

            yawConfig.X(data => data.TimeStamp.ToOADate());
            yawConfig.Y(data => data.CurrentAngle);

            tiltConfig.X(data => data.TimeStamp.ToOADate());
            tiltConfig.Y(data => data.CurrentAngle);

            YawSeries = new SeriesCollection(yawConfig)
            {
                new LineSeries
                {
                    Title = "Yaw",
                    Values = new ChartValues<ControllerData>(),
                    PointRadius = 1.5,
                    Stroke = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFB300"),
                    StrokeThickness = 1.5
                }
            };

            TiltSeries = new SeriesCollection(tiltConfig)
            {
                new LineSeries
                {
                    Title = "Tilt",
                    Values = new ChartValues<ControllerData>(),
                    PointRadius = 1.5,
                    Stroke = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFB300"),
                    StrokeThickness = 1.5
                }
            };

            XFormatter = val => DateTime.FromOADate(val).ToString("mm:ss");
            YFormatter = val => val + " °";

            yawChart.Series = YawSeries;
            yawChart.AxisX.LabelFormatter = XFormatter;
            yawChart.AxisY.LabelFormatter = YFormatter;

            tiltChart.Series = TiltSeries;
            tiltChart.AxisX.LabelFormatter = XFormatter;
            tiltChart.AxisY.LabelFormatter = YFormatter;
        }

        private void ResetPidCharts()
        {
            YawSeries.First().Values.Clear();
            TiltSeries.First().Values.Clear();
            yawChart.Redraw();
            tiltChart.Redraw();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource helicopterViewModelSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("helicopterViewModelSource")));
        }

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "YawControllerData")
            {
                if (YawSeries != null)
                {
                    Dispatcher.Invoke(() =>
                    {
                        var yawData = helicopterViewModel.SessionData.YawControllerData;
                        var yawSeries = YawSeries.First().Values;

                        if (yawSeries.Count > INT_NumSamplesPerWindow)
                        {
                            yawSeries.RemoveAt(0);
                        }

                        yawSeries.Add(yawData.Last());
                    });
                }
            }
            else if (e.PropertyName == "TiltControllerData")
            {
                if (TiltSeries != null)
                {
                    Dispatcher.Invoke(() =>
                    {
                        var tiltData = helicopterViewModel.SessionData.TiltControllerData;
                        var tiltSeries = TiltSeries.First().Values;

                        if (tiltSeries.Count > INT_NumSamplesPerWindow)
                        {
                            tiltSeries.RemoveAt(0);
                        }

                        tiltSeries.Add(tiltData.Last());
                    });
                }
            }
            else if (e.PropertyName == "IsSessionComplete")
            {
                ResetPidCharts();
            }
            else if (e.PropertyName == "IsConnected")
            {
                UpdateStatusBar();
            }
            else if (e.PropertyName == "OutputText")
            {
                controllerOutputTextbox.ScrollToEnd();
            }
        }
    }
}