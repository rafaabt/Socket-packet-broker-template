#ifndef UTILS__H
#define UTILS__H


#define EQ(A,B) !strcmp(A,B)

#define COMMAND(NAME) {#NAME, NAME ##_command}

typedef struct _Command
{
	const char *cmd;
	void (*function)(void);
} Command;


void quit_command ()
{
	printf("Quit\n");
}

void help_command ()
{
	printf("Help\n");
}


#if 0
Command commands[] = 
{
	{"quit", quit_command},
	{"help", help_command},
};
#else // equivalente
Command commands[] = 
{
	COMMAND(quit),
	COMMAND(help),
};

#endif

/*
Usage:
commands[0].function();
commands[1].function();
*/


#endif