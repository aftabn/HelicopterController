#ifndef UTIL_H_
#define UTIL_H_

void send(char *str);
void sendNack(void);
void sendAck(void);
void sendError(char* str);

bool isReadCommand(char* arg);
bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

int stricmp (const char *p1, const char *p2);

#endif