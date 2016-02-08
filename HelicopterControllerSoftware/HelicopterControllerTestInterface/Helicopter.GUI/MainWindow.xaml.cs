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
    public partial class MainWindow : Window
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly StartupOptions startupOptions;

        public MainWindow(StartupOptions startupOptions)
        {
            this.startupOptions = startupOptions;
            HelicopterViewModel = new HelicopterViewModel(startupOptions.IsDeveloperMode);

            InitializeComponent();

            SetBindingForControls();
        }

        public HelicopterViewModel HelicopterViewModel { get; set; }

        private void SetBindingForControls()
        {
            //controllerOutputPanel.DataContext = JigViewModel;
            //optionsToolbar.DataContext = JigViewModel;
            //jigStackPanel.DataContext = JigViewModel;
            //jigStackPanel2.DataContext = JigViewModel;
            //dynamixelStackPanel.DataContext = JigViewModel;
            //dynamixelStackPanel2.DataContext = JigViewModel;
            //menuHeader.DataContext = JigViewModel;
            //var position = JigViewModel.Servos[0].Position;
            //getPositionTextblock.DataContext = JigViewModel;
            //Binding myBinding = new Binding("PositionProperty");
            //myBinding.Source = JigViewModel.Servos[0].Position;
            //BindingOperations.SetBinding(getPositionTextblock, TextBlock.TextProperty, myBinding);
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource jigViewModelViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("jigViewModelViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            // jigViewModelViewSource.Source = [generic data source]
        }
    }
}