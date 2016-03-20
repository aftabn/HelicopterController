using Helicopter.Core.Controller;
using Libs.Utilities;
using log4net;
using System;
using System.IO;
using System.Xml.Linq;

namespace Helicopter.Core.Settings
{
    public class HelicopterSettings
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public HelicopterControllerSettings ControllerSettings { get; private set; }

        public string FilePath { get; set; }

        public XDocument XDocument { get; set; }

        public string XmlText { get; set; }

        public int PidThreadRefreshIntervalMilliseconds { get; set; }

        public ConnectionType ConnectionType { get; set; }

        /// <summary>
        /// Creates a settings file object
        /// </summary>
        /// <returns>The settings file with the parameters of each controller</returns>
        public static HelicopterSettings Load()
        {
            var settingsFile = GetSettingsFile();

            var helicopterSettings = new HelicopterSettings
            {
                FilePath = settingsFile,
                XmlText = File.ReadAllText(settingsFile),
            };

            helicopterSettings.XDocument = XDocument.Parse(helicopterSettings.XmlText);

            var settingsElement = helicopterSettings.XDocument.Element("Settings");
            var helicopterElement = settingsElement.Element("Helicopter");

            helicopterSettings.PidThreadRefreshIntervalMilliseconds = helicopterElement.Attribute("PidThreadRefreshIntervalMilliseconds").ParseInt();
            helicopterSettings.ConnectionType = (ConnectionType)Enum.Parse(typeof(ConnectionType), helicopterElement.Attribute("ConnectionType").Value);
            helicopterSettings.ControllerSettings = HelicopterControllerSettings.FromXmlElement(helicopterElement.Element("Controller"));

            return helicopterSettings;
        }

        private static string GetSettingsFile()
        {
            var executingAssemblyPath = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            var settingsFile = $"{executingAssemblyPath}\\{"HelicopterSettings.xml"}";
            Log.DebugFormat("Loading settings file: {0}", settingsFile);

            if (!File.Exists(settingsFile))
            {
                throw new Exception("Cannot find settings file: " + settingsFile);
            }

            return settingsFile;
        }
    }
}