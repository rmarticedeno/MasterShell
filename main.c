#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include ".config.h"

int size; //tamaño de cada línea a parsear empezando en 1
char* line; // línea a parsear
char* hostname; //nombre de la maquina
char* user; //nombre de usuario

char* wd; //directorio actual de trabajo

typedef struct cmd {
	char* id;
	int args;
	char** argv;
} command;

char** split(char *, const char *); //divide un string por un delimitador (repetido n veces n>=1)
command* split_commands(char*);

void vprint(char** s) {
	while(*s) {
		printf("%s ", *s);
		++s;
	}
	printf("\n");
}

void cprint(command* cmd) {
	while(cmd->id) {
		printf("%d ", cmd->args);
		vprint(cmd->argv);
		++cmd;
	}
	printf("Fin de los comandos\n");
}

int execute(command, int); //dada una linea parseada ejecuta el comando correspondiente
void init(); //inicializa todos los buffers

int main(int args, const char** argv)
{
    init();
    while(1)
    {
        getwd(wd); // obtener el directorio actual
        printf(format, user, hostname, wd); // format definido en .config.h
        getline(&line,&getln_a,stdin);
        command* cmds = split_commands(line);
    	cprint(cmds);
    	int fd = 0;
    	while(cmds->id) {
    		fd = execute(*cmds, fd);
    		++cmds;
    	}
    	char c;
    	while(read(fd, &c, 1)) { printf("%c", c); }
    	close(fd);
    }
	free(line);
}

void init()
{
    user = getlogin(); // inicializar el buffer de usuario
    hostname = malloc(ht_buffer * sizeof(char)); // inicializa el buffer de hostname
    line = malloc(ln_buffer * sizeof(char)); //inicializar la linea a parsear con tamaño ln_buffer definido en .config.h
    wd = malloc(wd_buffer * sizeof(char));  //inicializar el directorio con tamaño wd_buffer definido en .config.h
    gethostname(hostname,ht_buffer);
}

char** split(char* string, const char* pattr) // cuando hay espacios al final, size es uno mas de lo normal
{
    //inicializacines
    char** splitted = calloc(10 *sizeof(char*),sizeof(char*));
    splitted[0] = calloc(50*sizeof(char),sizeof(char));
    int cond = 0; // para evitar parsear líneas en blanco contínuas
    int start = 0; //para evitar parsear al inicio una línea en blanco
    int size1 = 10; //buffer de la cantidad de char*
    int size2 = 50; //buffer del tamaño de los char*
    int j = 0; // cantidad de char*
    
    for(int i = 0; *string; ++string)
    {
        if(i == size2)
        {
            splitted[i] = realloc(splitted[i] , (size2 + 50) * sizeof(char));
            size2 += 50;
        }
        if(*string == *pattr)
        {
            if(!cond && start) // si no es continuación de un espacio en blanco y no empieza por un espacio en blanco
            {
                if(j == size1)
                { 
                    splitted = realloc(splitted, (10 + size1)* sizeof(char*));
                    size1 += 10;
                }
                ++j;
                cond = 1;
            }
            i = 0; //reiniciar contador
        }
        else
        {
            if(splitted[j] == NULL) { splitted[j] = calloc(50*sizeof(char),sizeof(char)); }
            start = 1;
           // printf("%d\n", *string); descomentar para ver el numero del caracter a guradar (se incluye el fin de líne(numero 10) que posteriormente seera eliminado)
            if(*string != 10) //eliminar fin de línea (da problemas con la familia exec)
            {
                splitted[j][i] = *string;
                ++i;
                cond = 0;
            }
        }
    }
    if (splitted[j] == NULL) { size = j; } // total de divisiones empezando por 1
    else { size = j + 1; }
    return splitted;
}

command* split_commands(char* string) {
	char** cmds = split(string, "|");
	int total = size, i;
	command* commands = malloc((total + 1)* sizeof(command));
	
	for(i = 0; i < total; ++i) {
		commands[i].argv = split(cmds[i], " ");
		commands[i].args = size;
		commands[i].id = commands[i].argv[0];
	}
	commands[i].id = 0;
	return commands;
}

int execute(command cmd, int input_fd) //ejecutar un comando
{
	int p[2];
	pipe(p);
    if(!strcmp(cmd.argv[0], "cd")) //si es cd
    {
	close(p[1]);
        if(cmd.args != 2) // si existen mas de 2 argumentos imprimir error
        {
            printf("bash: cd: too many arguments\n");
            return - 1;
        }
        else
        {
            if(chdir(cmd.argv[1])) // si el chdir retorna error
            {
                perror("cd"); //magia :° imprime el mensaje del error con el encabezado que se le pase(cualquier error) :-!
            }
//             else
//             {
//                 printf("%s\n",getwd(wd));  // descomentar para imprimir el direcorio actual de trabajo
//             }
        }
    }
    else if(!strcmp(cmd.argv[0], "exit")) //si se escribe el comando exit salir de la consola
    {	
    	close(p[0]);
    	close(p[1]);
        free(line);
        free(wd);
        free(hostname);
        exit(0);
    }
    else
    {
        int pid = fork();
	    if (!pid)
	    {
    		dup2(input_fd, 0);
    		close(p[0]);
    		dup2(p[1], 1);
    		close(p[1]);
    		// primero sin considerar >, <, >>
    		char** par = malloc((cmd.args) * sizeof(char*));
    		int i;
    		for(i = 0; i <= cmd.args; ++i)
    		{
    		    if (i != cmd.args)
    		    {
    		        par[i] = cmd.argv[i];
    		    }
    		    else
    		    {
    		        par[i] = NULL;
    		    }
    		}
    		execvp(cmd.argv[0],par);
    		exit(EXIT_FAILURE);
	    }
	    else
	    {
    		int status;
    		wait(&status);
    		close(p[1]);
    		if(!WIFEXITED(status))
    		{
    		    perror(cmd.argv[0]); // imprime el error del comando correspondiente
    		}
	    }
    }
	return p[0];
}

