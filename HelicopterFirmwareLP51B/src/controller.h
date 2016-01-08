#ifndef CONTROLLER_H_
#define CONTROLLER_H_

void clearParameters(void);

void processCommand(char *command);
void processLine(char *str);
void processParameters(char *remainingString);

void send(char *str);
void sendNack(void);
void sendAck(void);
void sendError(char* str);
// void sendInt(int num);
// void sendDouble(double num);
// void sendOnOffStatus(bool isOn);
// void sendOneOrZeroStatus(bool isHigh);
// void sendSyntaxError(void);
// void sendReadOnlyError(void);
// void sendChannelError(void);
// void sendIntRangeError(int lowerLimit, int upperLimit, char* unit);
// void sendDoubleRangeError(double lowerLimit, double upperLimit, char* unit);
// void sendOnOffError(void);
// void sendOneOrZeroError(void);

void scanUSBPort(void);
// void updateTemperatureLight(void);
// void initController(void);

#endif