using log4net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading;

namespace Helicopter.Core.Controller
{
    public class CommunicationsManager : INotifyPropertyChanged, IDisposable
    {
        private const double DBL_DefaultTimeoutSeconds = 1;
        private const string STR_MainDeviceName = "Arduino";
        private const string STR_BluetoothDeviceName = "COM10";

        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly List<string> receivedPackets = new List<string>();

        private readonly object serialPortBufferLock = new object();
        private readonly object serialPortLock = new object();
        private readonly object receivedPacketsLock = new object();

        private SerialPort serialPort;
        private string serialPortBuffer;
        private string comPort = String.Empty;
        private bool isConnected;

        public CommunicationsManager()
        {
            serialPortBuffer = String.Empty;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsConnected
        {
            get
            {
                return isConnected;
            }

            private set
            {
                if (value != isConnected)
                {
                    isConnected = value;
                    RaisePropertyChanged("IsConnected");
                }
            }
        }

        public static List<string> GetSerialPortDescriptions()
        {
            log.Debug("Find serial ports with descriptive names.");
            var list = new List<string>();

            try
            {
                using (var searcher = new ManagementObjectSearcher("root\\CIMV2", "SELECT * FROM Win32_PnPEntity"))
                {
                    foreach (ManagementObject managementObjectCollection in searcher.Get())
                    {
                        if (managementObjectCollection["Caption"] != null)
                        {
                            var connectionDescription = managementObjectCollection["Caption"].ToString();

                            if (managementObjectCollection["Caption"].ToString().Contains("(COM"))
                            {
                                log.DebugFormat("Found COM device >{0}<", connectionDescription);
                                list.Add(connectionDescription);
                            }
                        }
                    }
                }
            }
            catch (ManagementException ex)
            {
                log.Error(ex.Message);
                throw;
            }

            return list;
        }

        public void Connect()
        {
            log.Debug("Connecting...");

            if (IsConnected)
            {
                return;
            }

            var serialPorts = GetSerialPortDescriptions();
            string potentialPort = String.Empty;
            string comPort = String.Empty;
            foreach (string port in serialPorts)
            {
                if (port.Contains(STR_MainDeviceName))
                {
                    potentialPort = port;
                    comPort = potentialPort.Substring(potentialPort.IndexOf("COM"));
                    comPort = comPort.Substring(0, comPort.Length - 1);
                    break;
                }
            }
            if (comPort != String.Empty)
            {
                serialPort = new SerialPort(comPort, 115200, Parity.None, 8, StopBits.One);
                serialPort.DataReceived += OnDataReceived;
                serialPort.Open();
                IsConnected = true;

                InitializeArduino();
            }
            else
            {
                throw new Exception(String.Format("Device \"{0}\" is not listed on the COM ports", STR_MainDeviceName));
            }
        }

        public void Disconnect()
        {
            log.Debug("Disconnecting...");

            if (IsConnected)
            {
                serialPort.DataReceived -= OnDataReceived;
                serialPort.Close();
                serialPort.Dispose();

                IsConnected = false;
            }

            serialPort = null;
        }

        public void Dispose()
        {
            Disconnect();
        }

        private void InitializeArduino()
        {
            // It apprears that there needs to be some delay after connecting to the Arduino
            // probably due to serial port initialization on its end
            Thread.Sleep(750);
            serialPort.Write("\r\n"); // Flushes whatever characters are already in the comport
            Thread.Sleep(750);
            ClearBuffer();
        }

        public Packet Write(string input)
        {
            return Write(input, DBL_DefaultTimeoutSeconds);
        }

        public Packet Write(string input, double timeoutSeconds)
        {
            lock (serialPortLock)
            {
                ClearReceivedPackets();

                try
                {
                    serialPort.Write(input + "\r\n");
                    log.DebugFormat("{0}\r\n");
                }
                catch (IOException e)
                {
                    var message = String.Format("Command {0} failed to send - could not communicate with the microcontroller.{0}The program will now terminate.", input, Environment.NewLine);
                    log.Debug(e.Message);
                    log.Debug(message);
                    throw new Exception(message);
                }

                return WaitForResponsePacket(input, timeoutSeconds);
            }
        }

        private void ClearBuffer()
        {
            lock (serialPortBufferLock)
            {
                serialPortBuffer = String.Empty;
            }
        }

        private void ClearReceivedPackets()
        {
            lock (receivedPacketsLock)
            {
                receivedPackets.Clear();
            }
        }

        private void AddNewPacket(string packet)
        {
            lock (receivedPacketsLock)
            {
                receivedPackets.Add(packet);
            }
        }

        private Packet WaitForResponsePacket(string input, double timeoutSeconds)
        {
            var startTime = DateTime.Now;

            while ((DateTime.Now - startTime).TotalSeconds < timeoutSeconds)
            {
                if (receivedPackets.Count() > 0)
                {
                    Packet packet;

                    lock (receivedPacketsLock)
                    {
                        packet = Packet.FromString(receivedPackets.First());
                    }

                    if (packet.Terminator == "ERROR")
                    {
                        var message = String.Format("Microcontroller returned \"{0}\" from command {1}. ", packet.ReturnValue, input);

                        log.Debug(message);
                        throw new Exception(message);
                    }

                    return packet;
                }

                Thread.Sleep(2);
            }
            throw new TimeoutException(String.Format("ComPort timed out waiting for response to input {0}.", input));
        }

        private void AddByteToBuffer(string newChar)
        {
            lock (serialPortBufferLock)
            {
                serialPortBuffer += newChar;
            }
        }

        private void OnDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var serialPort = (SerialPort)sender;
            var byteCount = serialPort.BytesToRead;

            for (int i = 0; i < byteCount; i++)
            {
                try
                {
                    var newChar = Encoding.ASCII.GetString(new byte[] { Convert.ToByte(serialPort.ReadByte()) });

                    AddByteToBuffer(newChar);

                    if (serialPortBuffer.EndsWith("OK\r\n") || serialPortBuffer.EndsWith("ERROR\r\n"))
                    {
                        AddNewPacket(serialPortBuffer);
                        var stringReceived = serialPortBuffer;

                        // Uncommenting the line below will log every packet received from the microcontroller
                        // log.DebugFormat("{0} <<< {1}", ((SerialPort)sender).PortName, stringReceived.Replace("\r\n", "<CRLF>"));
                        ClearBuffer();
                        return;
                    }
                }
                catch (Exception ex)
                {
                    var msg = String.Format("Exception reading byte from serial buffer. {0}", ex.Message);
                    log.Error(msg);
                    throw new Exception(msg);
                }
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}