using Helicopter.Core;
using Libs.Utilities;
using log4net;
using log4net.Config;
using Microsoft.Shell;
using SciChart.Charting.Visuals;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : ISingleInstanceApp
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly string applicationGuid = "{8AWBB3IT-A3F4-HU79-7GY5-4AJY99YTCXB0}"; // Random GUID
        private static StartupOptions startupOptions;

        // Single instance from: http://codereview.stackexchange.com/questions/20871/single-instance-wpf-application
        [STAThread]
        public static void Main(string[] argc)
        {
            if (SingleInstance<App>.InitializeAsFirstInstance(applicationGuid))
            {
                StartApplication(argc);

                // Allow single instance code to perform cleanup operations
                SingleInstance<App>.Cleanup();
            }
            else
            {
                var name = Assembly.GetEntryAssembly().GetName().Name;

                MessageBox.Show($"Unable to start {name}. Another instance of the application is already running.", name);
            }
        }

        public bool SignalExternalCommandLineArgs(IList<string> args)
        {
            // handle command line arguments of second instance ...

            return true;
        }

        private static void DisplayUnhandledException(System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e, string @event)
        {
            Log.Fatal("Unhandled exception: " + @event, e.Exception);
            MessageBox.Show(e.Exception.Message, @event, MessageBoxButton.OK, MessageBoxImage.Error);
            e.Handled = true;
        }

        private static void LogUnhandledException(Exception exception, string @event)
        {
            Log.Fatal("Unhandled exception: " + @event, exception);
            MessageBox.Show(exception.Message, @event, MessageBoxButton.OK, MessageBoxImage.Error);
        }

        private static void StartApplication(string[] argc)
        {
            startupOptions = new StartupOptions(argc);

            InitializeLog4Net();
            InitializeSciChartLicense();

            try
            {
                var app = new App();
                InitializeExceptionHandling(app);

                app.MainWindow = new HelicopterControllerWindow(startupOptions);
                app.MainWindow.Show();
                app.Run();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Unhandled exception", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private static void InitializeExceptionHandling(App app)
        {
            // Exception handling for MVVM
            AppDomain.CurrentDomain.UnhandledException += (s, ex) =>
                LogUnhandledException((Exception)ex.ExceptionObject, "AppDomain.CurrentDomain.UnhandledException");

            app.DispatcherUnhandledException += (s, e) =>
                DisplayUnhandledException(e, "Application.Current.DispatcherUnhandledException");

            TaskScheduler.UnobservedTaskException += (s, ex) =>
                LogUnhandledException(ex.Exception, "TaskScheduler.UnobservedTaskException");
        }

        private static void InitializeLog4Net()
        {
            try
            {
                // Creates folders for log files (log4net)
                var commonApplicationData = new CommonApplicationData(true);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }

            XmlConfigurator.Configure();
            var entryAssembly = $"{Assembly.GetEntryAssembly().GetName().Name} {Assembly.GetEntryAssembly().GetName().Version}";
            Log.Debug(entryAssembly + " starting up.");
        }

        private static void InitializeSciChartLicense()
        {
            SciChartSurface.SetRuntimeLicenseKey(@"<LicenseContract>
              <Customer>University of British Columbia</Customer>
              <OrderId>EDU-UBC-001</OrderId>
              <LicenseCount>1</LicenseCount>
              <IsTrialLicense>false</IsTrialLicense>
              <SupportExpires>04/19/2017 00:00:00</SupportExpires>
              <ProductCode>SC-WPF-SDK-PRO</ProductCode>
              <KeyCode>lwAAAAEAAADZIEr+JJrRAXYAQ3VzdG9tZXI9VW5pdmVyc2l0eSBvZiBCcml0aXNoIENvbHVtYmlhO09yZGVySWQ9RURVLVVCQy0wMDE7U3Vic2NyaXB0aW9uVmFsaWRUbz0xOS1BcHItMjAxNztQcm9kdWN0Q29kZT1TQy1XUEYtU0RLLVBST35t4JULJmi37YVDUku120oMpF+A3eK3IQ0SDZbwEQ17fJRdTWcBuW2M+cT3oTEKgw==</KeyCode>
            </LicenseContract>");
        }
    }
}