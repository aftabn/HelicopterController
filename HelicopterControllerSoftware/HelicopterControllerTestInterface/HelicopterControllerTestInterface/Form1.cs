using HelicopterController.Core;
using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HelicopterControllerTestInterface
{
    public partial class Form1 : Form
    {
        private const bool isEngMode = true;
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private Microcontroller microcontroller = new ArduinoMicrocontroller(isEngMode);

        public Form1()
        {
            InitializeComponent();
            microcontroller.Connect();
        }
    }
}