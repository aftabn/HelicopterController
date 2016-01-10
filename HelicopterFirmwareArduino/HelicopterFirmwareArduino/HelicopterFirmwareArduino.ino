/*
 Name:		HelicopterFirmwareArduino.ino
 Created:	1/10/2016 1:09:10 PM
 Author:	Aftab
*/

int incomingByte;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	if (Serial.available() > 0) {
		// read the incoming byte:
		incomingByte = Serial.read();

		// say what you got:
		Serial.print("I received: ");
		Serial.println(incomingByte, DEC);
	}
}