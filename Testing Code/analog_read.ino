void setup()
{
  Serial.begin(115200);
}

void loop()
{
  int voltage = analogRead(A0);
  double analogVoltage = voltage / 1023.0 * 5;
  Serial.print("Voltage is: ");
  Serial.println(analogVoltage);
  delay(100);
}
