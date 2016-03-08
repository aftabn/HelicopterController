using Helicopter.Core.Sessions;
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
using System.Windows.Shapes;

namespace Helicopter.GUI.PidCharts
{
    /// <summary>
    /// Interaction logic for SessionPidChartWindow.xaml
    /// </summary>
    public partial class SessionPidChartWindow : Window
    {
        public SessionPidChartWindow(Session session)
        {
            InitializeComponent();

            staticPidChart.LoadNewData(session);
        }
    }
}