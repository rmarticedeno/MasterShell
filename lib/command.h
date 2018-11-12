#if !defined printer
#define printer

typedef struct cmd {
	char* id;
	int args;
	char** argv;
	int final;
} command;

void print_char_vector(char**);

void print_command(command*);

char** split(char*, const char*);

command* split_commands(char*);

#endif