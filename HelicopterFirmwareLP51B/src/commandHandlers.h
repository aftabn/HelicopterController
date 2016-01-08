#ifndef COMMANDHANDLERS_H_
#define COMMANDHANDLERS_H_

void onCommandEcho(void);
void onCommandIdentity(void);
void onCommandSystem(void);
void onCommandHelp(void);
void handleCommandUnknown(char *command);

#endif /* COMMANDHANDLERS_H_ */