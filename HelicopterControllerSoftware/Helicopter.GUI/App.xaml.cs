using Helicopter.Core;
using Libs.Utilities;
using log4net;
using log4net.Config;
using SciChart.Charting.Visuals;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows;

namespace Helicopter.GUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private StartupOptions startupOptions;

        private void Application_Startup(object sender, StartupEventArgs startupEventArgs)
        {
            try
            {
                // Creates folders for log files (log4net)
                var commonApplicationData = new CommonApplicationData(allUsers: true);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }

            SciChartSurface.SetRuntimeLicenseKey(@"<LicenseContract>
                <Customer>aftab@narsimhan.com</Customer>
                <OrderId>University Scheme</OrderId>
                <LicenseCount>1</LicenseCount>
                <IsTrialLicense>true</IsTrialLicense>
                <SupportExpires>04/26/2016 00:00:00</SupportExpires>
                <KeyCode>tgIAAA0jQpSob9EBAEBrk06f0QE9AGMAQ3VzdG9tZXI9YWZ0YWJAbmFyc2ltaGFuLmNvbTtPcmRlcklkPVVuaXZlcnNpdHkgU2NoZW1lO1N1YnNjcmlwdGlvblZhbGlkVG89MjYtQXByLTIwMTY7UHJvZHVjdENvZGU9aUveVZfOqg7TtoBg2HGP1db8TupiPw4sxKI4YoiJ/W+BH1kLdOsXwOp5vMarlGBC</KeyCode>
            </LicenseContract>");

            // Exception handling for MVVM
            AppDomain.CurrentDomain.UnhandledException += (s, ex) =>
                LogUnhandledException((Exception)ex.ExceptionObject, "AppDomain.CurrentDomain.UnhandledException");

            DispatcherUnhandledException += (s, e) =>
                DisplayUnhandledException(e, "Application.Current.DispatcherUnhandledException");

            TaskScheduler.UnobservedTaskException += (s, ex) =>
                LogUnhandledException(ex.Exception, "TaskScheduler.UnobservedTaskException");

            XmlConfigurator.Configure();
            var entryAssembly = String.Format("{0} {1}", Assembly.GetEntryAssembly().GetName().Name, Assembly.GetEntryAssembly().GetName().Version);
            log.Debug(entryAssembly + " starting up.");

            startupOptions = new StartupOptions(startupEventArgs.Args);
            var window = new HelicopterControllerWindow(startupOptions);
            window.Show();
        }

        private void DisplayUnhandledException(System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e, string @event)
        {
            log.Fatal("Unhandled exception: " + @event, e.Exception);
            MessageBox.Show(e.Exception.Message, @event, MessageBoxButton.OK, MessageBoxImage.Error);
            e.Handled = true;
        }

        private void LogUnhandledException(Exception exception, string @event)
        {
            log.Fatal("Unhandled exception: " + @event, exception);
            MessageBox.Show(exception.Message, @event, MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }
}