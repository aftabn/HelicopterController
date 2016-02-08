using Helicopter;
using log4net;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

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