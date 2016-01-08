#include <controller.h>
#include <commandHandlers.h>
#include <util.h>
#include <globals.h>

void processCommand(char *command)
{
	if (0 == strcmp(command, "*IDN?"))
	{
		onCommandIdentity();
	}
	else if (0 == strcmp(command, "ECHO"))
	{
		onCommandEcho();
	}
	else if (0 == strcmp(command, "SYS"))
	{
		onCommandSystem();
	}
	else if (0 == strcmp(command, "HELP"))
	{
		onCommandHelp();
	}
	else
	{
		handleCommandUnknown(command);
	}
}


void clearParameters()
{
	for (int i = 0; i < INT_PARAMETER_COUNT_MAX; i++)
	{
		strcpy(gParameters[i], "");
	}
}

void processParameters(char *parameterString)
{
	clearParameters();

	if (NULL != parameterString && strlen(parameterString))
	{
		int i = 0;
		const char delimiter[] = " ";
		char *token;
		
		// break up remaining strings into parameters
		token = strtok(parameterString, delimiter);
		
		while (NULL != token)
		{
			// copy parameters into the global parameter variables
			strcpy(gParameters[i++], token);
			token = strtok(NULL, delimiter);
		}
	}
}

void processLine(char *line)
{
	char buffer[256];
	const char delimiter[] = " ";
	char *commandString = NULL;
	char *remainingString = NULL;
	
	strcpy(buffer, line);
	upperCaseString(buffer);
	
	// get first token
	commandString = strtok(buffer, delimiter);
	
	if (NULL != commandString)
	{
		if (strlen(line) - strlen(commandString) > 0)
		{
			remainingString = line + strlen(commandString) + 1;
		}
		
		processParameters(remainingString);
		processCommand(commandString);
	}
}

void scanUSBPort()
{
	char tmpstr[256];
	
	while (true)
	{
		//pulseStatusLed();
		scanf("> %s", tmpstr);
		send(tmpstr);
		processLine(tmpstr);
	}
}