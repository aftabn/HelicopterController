
#ifndef COMMANDHANDLERS_H_
#define COMMANDHANDLERS_H_

void onCommandEcho(void);
void onCommandIdentity(void);
void onCommandSystem(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

bool isReadCommand(char* arg);
bool isOnCommandArg(char* arg);
bool isOffCommandArg(char* arg);
bool isChannelCorrect(char* channelArg);
bool isIntWithinRange(int number, int lowerLimit, int upperLimit);
bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit);

int stricmp (const char *p1, const char *p2);



#endif /* COMMANDHANDLERS_H_ */