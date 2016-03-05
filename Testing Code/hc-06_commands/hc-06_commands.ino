// From: http://www.instructables.com/id/AT-command-mode-of-HC-05-Bluetooth-module/?ALLSTEPS
// This is used to write commands to the HC-06 bluetooth module
// Connect pins 10 and 11 to the bluetooth chip and send commands
// via serial port monitor WITHOUT line endings

// Commands:
//AT : Check the connection
//AT+NAMEX: Set name by replacing X
//AT+BAUDX: Set baudrate by replacing X (see below)
//AT+VERSION : Get version

// Baudrates:
//1 set to 1200bps
//2 set to 2400bps 
//3 set to 4800bps 
//4 set to 9600bps (Default) 
//5 set to 19200bps 
//6 set to 38400bps 
//7 set to 57600bps 
//8 set to 115200bps

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() 
{
  Serial.begin(115200);
  Serial.println("Enter AT commands:");
  mySerial.begin(19200);
}

void loop()
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}
