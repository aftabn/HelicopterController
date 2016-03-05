#include <Streaming.h>

#define ENCODER_CHA_PIN 4
#define ENCODER_CHB_PIN 5

#include <digitalWriteFast.h>

const signed int encoderLookup[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
int currentAngle = 0;
int encoderValues;
unsigned long delta;
bool timeFlag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initializeQuadratureDecoder();
}

void loop() {
  if (timeFlag)
  {
    Serial << "Time: " << delta << ", Angle: " << currentAngle << "\r\n";
    timeFlag = false;
  }
}

void initializeQuadratureDecoder(void)
{
  pinModeFast(ENCODER_CHA_PIN, INPUT);
  pinModeFast(ENCODER_CHB_PIN, INPUT);
  encoderValues = (digitalReadFast(ENCODER_CHA_PIN) << 1) | digitalReadFast(ENCODER_CHB_PIN);

  // Sets ISR for external interrupt on pin 2
  attachInterrupt(0, quadratureDecoderISR, RISING);
}

static void quadratureDecoderISR(void)
{
  unsigned long start = micros();
  
  encoderValues <<= 2;
  encoderValues |= ((digitalReadFast(ENCODER_CHA_PIN) << 1) | digitalReadFast(ENCODER_CHB_PIN));
  currentAngle += encoderLookup[encoderValues & 0x0F] * 0.9;
  
  unsigned long endTime = micros();
  delta = endTime - start;
  timeFlag = true;
}
