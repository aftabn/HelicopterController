using Helicopter.Core;
using Helicopter.Core.Sessions;
using LiveCharts;
using LiveCharts.CoreComponents;
using log4net;
using System;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class HelicopterControllerWindow : Window, INotifyPropertyChanged
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private const int INT_NumSamplesPerWindow = 75;
        private readonly StartupOptions startupOptions;
        private HelicopterViewModel helicopterViewModel;

        public HelicopterControllerWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            helicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);
            helicopterViewModel.PropertyChanged += OnViewModelPropertyChanged;

            InitializeComponent();
            SetVisibilty();
            SetBindingForControls();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void SetVisibilty()
        {
            Width = 600;
            controllerOutputPanel.Visibility = Visibility.Collapsed;
            Grid.SetRow(yawPidChart, 0);
            Grid.SetRow(tiltPidChart, 1);
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

        private void SetBindingForControls()
        {
            menuHeader.DataContext = helicopterViewModel;
            optionsToolbar.DataContext = helicopterViewModel;
            controllerTabs.DataContext = helicopterViewModel;
            controllerOutputPanel.DataContext = helicopterViewModel;
        }

        private void StartPidCharting()
        {
            yawPidChart.StartNewSession(helicopterViewModel.HelicopterManager.Session.StartTime, helicopterViewModel.HelicopterManager.Session.YawDataSeries);
            tiltPidChart.StartNewSession(helicopterViewModel.HelicopterManager.Session.StartTime, helicopterViewModel.HelicopterManager.Session.TiltDataSeries);
        }

        private void StopPidCharting()
        {
            yawPidChart.EndSession();
            tiltPidChart.EndSession();
        }

        private void OnOperatorTabGetFocus(object sender, RoutedEventArgs e)
        {
            Width = 600;
        }

        private void OnDeveloperTabGetFocus(object sender, RoutedEventArgs e)
        {
            Width = 1500;
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource helicopterViewModelSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("helicopterViewModelSource")));
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

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsConnected")
            {
                UpdateStatusBar();
            }
            else if (e.PropertyName == "IsSessionRunning")
            {
                if (helicopterViewModel.IsPidSessionRunning)
                {
                    StartPidCharting();
                }
                else
                {
                    StopPidCharting();
                }
            }
            else if (e.PropertyName == "OutputText")
            {
                controllerOutputTextbox.ScrollToEnd();
            }
        }
    }
}