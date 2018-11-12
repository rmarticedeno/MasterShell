#if !defined hcommand
#define hcommand

typedef struct cmd {
	char* id;
	int args;
	char** argv;
	int start;
	int final;
	int bg;
} command;

void print_char_vector(char**);

void print_command(command*);

char** split(char*, const char*);

int is_pipe(char);

char* space_pipes(char*);

command* split_commands(char*);

#endif
