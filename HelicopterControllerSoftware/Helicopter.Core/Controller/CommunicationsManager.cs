﻿using log4net;
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
        private const double DBL_DefaultSerialTimeoutSeconds = 0.5;
        private const double DBL_DefaultBluetoothTimeoutSeconds = 3;
        private const string STR_SerialDeviceName = "Arduino";
        private const string STR_BluetoothDeviceName = "Bluetooth";
        private const int INT_SerialBaudRate = 19200;
        private const int INT_BluetoothBaudRate = 115200;

        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly List<string> receivedPackets = new List<string>();

        private readonly object serialPortBufferLock = new object();
        private readonly object serialPortLock = new object();
        private readonly object receivedPacketsLock = new object();

        private ConnectionType connectionType;
        private SerialPort serialPort;
        private string deviceName;
        private string serialPortBuffer = String.Empty;
        private string comPort = String.Empty;
        private bool isConnected;

        public CommunicationsManager(ConnectionType defaultConnectionType)
        {
            ConnectionType = defaultConnectionType;
            Log.Debug("Created new CommunicationsManager");
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
                if (value == isConnected) return;

                isConnected = value;
                RaisePropertyChanged("IsConnected");
            }
        }

        public ConnectionType ConnectionType
        {
            get
            {
                return connectionType;
            }

            set
            {
                if (value == connectionType) return;

                connectionType = value;
                RaisePropertyChanged("ConnectionType");
            }
        }

        public static List<string> GetSerialPortDescriptions()
        {
            Log.Debug("Find serial ports with descriptive names.");
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
                                Log.DebugFormat("Found COM device >{0}<", connectionDescription);
                                list.Add(connectionDescription);
                            }
                        }
                    }
                }
            }
            catch (ManagementException ex)
            {
                Log.Error(ex.Message);
                throw;
            }

            return list;
        }

        public void Connect()
        {
            Log.Debug("Connecting...");

            if (IsConnected)
            {
                return;
            }

            SetDeviceName();

            var serialPorts = GetSerialPortDescriptions();
            foreach (var port in serialPorts)
            {
                if (port.Contains(deviceName))
                {
                    comPort = port.Substring(port.IndexOf("COM"));
                    comPort = comPort.Substring(0, comPort.Length - 1);
                    break;
                }
            }
            if (comPort != String.Empty)
            {
                var baudRate = ConnectionType == ConnectionType.Serial ? INT_SerialBaudRate : INT_BluetoothBaudRate;

                serialPort = new SerialPort(comPort, baudRate, Parity.None, 8, StopBits.One);
                serialPort.DataReceived += OnDataReceived;
                serialPort.Open();
                IsConnected = true;

                if (ConnectionType == ConnectionType.Serial)
                {
                    InitializeArduino();
                }

                Log.DebugFormat("Connected to controller through {0} on {1}", ConnectionType, comPort);
            }
            else
            {
                throw new Exception($"Device \"{deviceName}\" is not listed on the COM ports");
            }
        }

        public void Disconnect()
        {
            Log.Debug("Disconnecting...");

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
            serialPort.Write("\r\n"); // Flushes whatever characters are already in the Arduino serial buffer
            Thread.Sleep(750);
            ClearBuffer();
        }

        private void SetDeviceName()
        {
            switch (ConnectionType)
            {
                case ConnectionType.Serial:
                    deviceName = STR_SerialDeviceName;
                    break;

                case ConnectionType.Bluetooth:
                    deviceName = STR_BluetoothDeviceName;
                    break;

                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public Packet Write(string input)
        {
            var timeout = ConnectionType == ConnectionType.Serial ? DBL_DefaultSerialTimeoutSeconds : DBL_DefaultBluetoothTimeoutSeconds;
            return Write(input, timeout);
        }

        public Packet Write(string input, double timeoutSeconds)
        {
            lock (serialPortLock)
            {
                ClearReceivedPackets();

                try
                {
                    serialPort.Write(input + "\r\n");
                    Log.DebugFormat("Sending <{0}>", input);
                }
                catch (IOException e)
                {
                    var message = $"Command {input} failed to send - could not communicate with the microcontroller.{Environment.NewLine}" + $"The program will now terminate.";
                    Log.Debug(e.Message);
                    Log.Debug(message);
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
                if (receivedPackets.Any())
                {
                    Packet packet;

                    lock (receivedPacketsLock)
                    {
                        packet = Packet.FromString(receivedPackets.First());
                    }

                    if (packet.Terminator == "ERROR")
                    {
                        var message = $"Microcontroller returned \"{packet.ReturnValue}\" from command {input}. ";

                        Log.Debug(message);
                        throw new Exception(message);
                    }

                    return packet;
                }

                Thread.Sleep(2);
            }
            throw new TimeoutException($"ComPort timed out waiting for response to input {input}.");
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

                        // Uncommenting the lines below will log every packet received from the microcontroller
                        //var stringReceived = serialPortBuffer;
                        // log.DebugFormat("{0} <<< {1}", ((SerialPort)sender).PortName, stringReceived.Replace("\r\n", "<CRLF>"));
                        ClearBuffer();
                        return;
                    }
                }
                catch (Exception ex)
                {
                    var msg = $"Exception reading byte from serial buffer. {ex.Message}";
                    Log.Error(msg);
                    throw new Exception(msg);
                }
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}