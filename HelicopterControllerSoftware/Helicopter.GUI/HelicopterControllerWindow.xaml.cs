using Helicopter.Core;
using log4net;
using System.Windows;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class HelicopterControllerWindow : Window
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly StartupOptions startupOptions;

        public HelicopterControllerWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            HelicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);

            InitializeComponent();
            SetBindingForControls();
        }

        public HelicopterViewModel HelicopterViewModel { get; set; }

        private void SetBindingForControls()
        {
            menuHeader.DataContext = HelicopterViewModel;
            optionsToolbar.DataContext = HelicopterViewModel;
            controllerTabs.DataContext = HelicopterViewModel;
            controllerOutputPanel.DataContext = HelicopterViewModel;
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource helicopterViewModelSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("helicopterViewModelSource")));
        }
    }
}