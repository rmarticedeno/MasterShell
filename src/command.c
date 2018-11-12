#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"

int size;

void print_char_vector(char** s) {
	printf("---- Argv: ");
	while(*s) {
		printf("%s ", *s);
		++s;
	}
	printf("\n\n");
}

void print_command(command* cmd) {
	printf("\n===    Listando Comandos..    ===\n");
	while(cmd->id) {
		printf("\nComando: %s\n---- Args: %d\n", cmd->id, cmd->args);
		print_char_vector(cmd->argv);
		++cmd;
	}
	printf("===    Fin de los comandos    ===\n\n");
}

char** split(char* input, const char* separator) {
    int i, j, n = strlen(input); // verifica los espacios que no indican separacion
    for(i = 1; i < n; ++i) { if(input[i] == *separator && input[i - 1] == '\\') { input[i] = '\a'; } }
	int total = 8;
    char** splt = calloc((total + 1) * sizeof(char*), sizeof(char*));
    char* parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {

    	if(index == total) {
    		total += 8;
    		splt = realloc(splt, (total + 1) * sizeof(char*)); 
    	}

        n = strlen(parsed);
        splt[index] = calloc((n + 1) * sizeof(char), sizeof(char));
        for(i = 0, j = 0; i < n; ++i) { 
        	if(parsed[i] == '\a') { splt[index][j - 1] = *separator; }
        	else { splt[index][j++] = parsed[i]; } 
        }
        index++;

        parsed = strtok(NULL, separator);
    }

    splt[index] = NULL;
    size = index;

    n = strlen(input);
    for(i = 0; i < n; ++i) { if(input[i] == '\a') { input[i] = *separator; } }

    return splt;
}

int is_pipe(char c) { return c == '|' || c == '<' || c == '>'; }

char* space_pipes(char* string) {
	int n = strlen(string);
	char* s = calloc(2 * n * sizeof(char), sizeof(char));
	
	int i, j;
	for(i = j = 0; i < n; ++i) {
		if(i && (is_pipe(string[i]) ^ is_pipe(string[i - 1]))) { s[j++] = ' '; }
		s[j++] = string[i];
	}
	
	s = realloc(s, ++j * sizeof(char));
	return s;
}

command* split_commands(char* string) {
	char** cmds = split(string, "|");
	int total = size, i;
	command* commands = malloc((total + 1)* sizeof(command));
	
	for(i = 0; i < total; ++i) {
		commands[i].argv = split(space_pipes(cmds[i]), " ");
		free(cmds[i]);
		commands[i].args = size;
		commands[i].id = commands[i].argv[0];
		commands[i].final = commands[i].start = 0;
		commands[i].bg = 0;
	}
	commands[0].start = 1;
	commands[i - 1].final = 1;
	commands[i].id = 0;
	return commands;
}
