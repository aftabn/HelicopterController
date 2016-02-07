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
        public MainWindow()
        {
            InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            System.Windows.Data.CollectionViewSource jigViewModelViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("jigViewModelViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            // jigViewModelViewSource.Source = [generic data source]
        }
    }
}