int sampleInterval = 10;
long previousMill = 0;

long currentPosition = 0;
bool previousEncoderA, previousEncoderB;
bool currentEncoderA, currentEncoderB;
bool isForward = false;
bool isError = false;

void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(4, INPUT); 
  pinMode(5, INPUT); 
  previousEncoderA = (bool)digitalRead(4);
  previousEncoderB = (bool)digitalRead(5);
  attachInterrupt(0, blink, RISING);
}

void loop()
{
  unsigned long currentMill = millis();

  if (currentMill - previousMill > sampleInterval)
  {
    previousMill = currentMill;
    Serial.print("Position is: ");
    Serial.println(currentPosition);
  }
}

void blink()
{
  currentEncoderA = (bool)digitalRead(4);
  currentEncoderB = (bool)digitalRead(5);
  
  if(!previousEncoderA && !previousEncoderB) {
    isForward = currentEncoderA;
  } else if (previousEncoderA && !previousEncoderB) {
    isForward = currentEncoderB;
  } else if (previousEncoderA && previousEncoderB) {
    isForward = !currentEncoderA;
  } else if (!previousEncoderA && previousEncoderB) {
    isForward = !currentEncoderB;
  } else {
    isError = true;
  }

  if (!isError)
  {
    currentPosition += isForward ? 1 : -1;
  }

  previousEncoderA = currentEncoderA;
  previousEncoderB = currentEncoderB;
}