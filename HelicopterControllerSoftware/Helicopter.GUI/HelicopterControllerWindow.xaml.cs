using Helicopter.Core;
using log4net;
using System.ComponentModel;
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
        private readonly StartupOptions startupOptions;
        private HelicopterViewModel helicopterViewModel;

        public HelicopterControllerWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            helicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);
            helicopterViewModel.PropertyChanged += OnViewModelPropertyChanged;

            InitializeComponent();
            SetBindingForControls();
        }

        public event PropertyChangedEventHandler PropertyChanged;

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

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource helicopterViewModelSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("helicopterViewModelSource")));
        }

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsConnected")
            {
                UpdateStatusBar();
            }
        }
    }
}