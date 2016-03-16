using Helicopter.Core;
using Helicopter.Core.Sessions;
using Helicopter.GUI.PidCharts;
using Helicopter.Model;
using log4net;
using System;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class HelicopterControllerWindow : Window, INotifyPropertyChanged
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly StartupOptions startupOptions;
        private HelicopterViewModel helicopterViewModel;
        private SessionPidChartWindow sessionPidChartWindow;

        public HelicopterControllerWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            helicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);
            helicopterViewModel.PropertyChanged += OnViewModelPropertyChanged;

            InitializeComponent();
            SetBindingForControls();
            InitializeDatabaseDataGrid();
            SetVisibilty();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void SetVisibilty()
        {
            ControllerOutputPanel.Visibility = Visibility.Collapsed;
        }

        private void UpdateStatusBar()
        {
            SolidColorBrush statusBarColor;

            //if (helicopterViewModel.IsConnected)
            //{
            //    helicopterControllerStatusTextbox.Text = "Helicopter Controller Connected";
            //    statusBarColor = new SolidColorBrush(Color.FromRgb(0x00, 0x7A, 0xCC)); // Visual Studio Blue
            //}
            //else
            //{
            //    helicopterControllerStatusTextbox.Text = "Helicopter Controller Not Connected";
            //    statusBarColor = Brushes.Firebrick;
            //}

            if (helicopterViewModel.IsConnected)
            {
                HelicopterControllerStatusTextbox.Text = "Helicopter Controller Connected";

                if (helicopterViewModel.IsDatabaseConnected)
                {
                    DatabaseLiveTextBlock.Text = "Database Connected";
                    statusBarColor = new SolidColorBrush(Color.FromRgb(0x00, 0x7A, 0xCC)); // Visual Studio Blue
                }
                else
                {
                    statusBarColor = Brushes.DarkGoldenrod;
                }
            }
            else if (helicopterViewModel.IsDatabaseConnected)
            {
                DatabaseLiveTextBlock.Text = "Database Connected";
                statusBarColor = Brushes.DarkGoldenrod;
            }
            else
            {
                HelicopterControllerStatusTextbox.Text = "Helicopter Controller Not Connected";
                DatabaseLiveTextBlock.Text = "Database Not Connected";
                statusBarColor = Brushes.Firebrick;
            }

            StatusBar.Background = statusBarColor;
        }

        private void SetBindingForControls()
        {
            MenuHeader.DataContext = helicopterViewModel;
            OptionsToolbar.DataContext = helicopterViewModel;
            ControllerTabs.DataContext = helicopterViewModel;
            ControllerOutputPanel.DataContext = helicopterViewModel;
            DatabaseDataGrid.DataContext = helicopterViewModel;
        }

        private void InitializeDatabaseDataGrid()
        {
            Style rowStyle = new Style(typeof(DataGridRow));
            rowStyle.Setters.Add(new EventSetter(DataGridRow.MouseDoubleClickEvent, new MouseButtonEventHandler(OnRowDoubleClick)));
            DatabaseDataGrid.RowStyle = rowStyle;

            helicopterViewModel.InitializeDatabase();
        }

        private void OnRowDoubleClick(object sender, MouseButtonEventArgs e)
        {
            var recordId = helicopterViewModel.SelectedRecord != null ? helicopterViewModel.SelectedRecord.Id : -1;

            using (var context = new HelicopterModelEntities())
            {
                var sessionRecord = DatabaseManager.GetSessionRecord(recordId, context);
                sessionPidChartWindow = new SessionPidChartWindow(sessionRecord);
                sessionPidChartWindow.Show();
            }
        }

        private void StartPidCharting()
        {
            PidChart.StartNewSession(helicopterViewModel.HelicopterManager.Session);
        }

        private void StopPidCharting()
        {
            PidChart.EndSession();
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
                        helicopterViewModel.HelicopterManager.StopSession();
                        helicopterViewModel.Disconnect();
                    }

                    if (sessionPidChartWindow != null)
                    {
                        sessionPidChartWindow.Close();
                    }

                    break;
            }
        }

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsConnected" || e.PropertyName == "IsDatabaseConnected")
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
            else if (e.PropertyName == "IsSessionComplete")
            {
                if (helicopterViewModel.HelicopterManager.IsSessionComplete)
                {
                    var sessionRecord = DatabaseManager.CreateNewSessionRecord(helicopterViewModel.HelicopterManager.Session, helicopterViewModel.HelicopterManager.HelicopterSettings);
                    sessionPidChartWindow = new SessionPidChartWindow(sessionRecord);
                    sessionPidChartWindow.Show();
                }
            }
            else if (e.PropertyName == "OutputText")
            {
                ControllerOutputTextbox.ScrollToEnd();
            }
        }

        private void OnHistoryTabGotFocus(object sender, RoutedEventArgs e)
        {
            PidChart.Visibility = Visibility.Hidden;
        }

        private void OnHistoryTabLostFocus(object sender, RoutedEventArgs e)
        {
            PidChart.Visibility = Visibility.Visible;
        }
    }
}