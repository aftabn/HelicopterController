

void send(char* str)
{
	printf("%s\r\n", str);
}

void sendNack()
{
	send("ERROR");
}

void sendAck()
{
	send("OK");
}

void sendError(char* str)
{
	send(str);
	sendNack();
}

bool isReadCommand(char* arg)
{
	return (0 == stricmp(arg, ""));
}

bool isIntWithinRange(int number, int lowerLimit, int upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

bool isDoubleWithinRange(double number, double lowerLimit, double upperLimit)
{
	return (number >= lowerLimit && number <= upperLimit);
}

int stricmp (const char *p1, const char *p2)
{
	register unsigned char *s1 = (unsigned char *) p1;
	register unsigned char *s2 = (unsigned char *) p2;
	unsigned char c1, c2;
	
	do
	{
		c1 = (unsigned char) toupper((int)*s1++);
		c2 = (unsigned char) toupper((int)*s2++);
		if (c1 == '\0')
		{
			return c1 - c2;
		}
	}
	while (c1 == c2);
	
	return c1 - c2;
}