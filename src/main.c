#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "command.h"
#include "history.h"
#include "queue.h"
#include ".config.h"
#include "background.h"

char* line; // línea a parsear
char* rline; //linea con los cambios del history
char* hostname; // nombre de la maquina
char* user; // nombre de usuario
char* wd; // directorio actual de trabajo
history* h; // historial
queue* childs; // los pids de los procesos hijos
int last_status; // status del ultimo hijo
int gpid,pgpid; // gpid del proceso a guardar en background
int backgpid; // gpid del proceso en background que se le hizo wait
int bg; //1 si se tiene que ejecutar el comando en background
int changed; //si getline devolvio error
int finish; //terminar de hacer wait a el proceso

void init(); // inicializa todos los buffers
void welcome(); // mensaje de bienvenida
void quit(int); // terminar
int execute(command*, int); //dada una linea parseada ejecuta el comando correspondiente
int check_bg(char*); // para varificar si hay que madar los procesos al background
int IsNumber(char*); // para saber si el char* es un numero
typedef void(*sighandler_t)(int);
void handler(int); // para controlar las interrupciones

int main(int args, const char** argv) {
    init();

    while(1) {
        gpid = pgpid = backgpid = -1;
        bg = finish = 0;
        getcwd(wd, wd_buffer); // obtener el directorio actual
        if(!changed)
        {
            printf(format, user, hostname, wd); // format definido en .config.h
        }
        else
        {
            changed = 0;
        }
        if(getline(&line, &getln_a, stdin) == -1) { changed = 1; continue; }

        char** sline = split(line, "\n"); // remover fin de linea

        if(*sline) {
            char* herror;
            char* pline = process_line(h, *sline, &herror); // procesar los comandos de reutilizacion
            
            if(!strlen(herror)) { // si no hay errores
                rline = calloc((strlen(pline) + 1) * sizeof(char), sizeof(char));
                strcpy(rline, pline);
                if(strcmp(*sline, pline)) { printf("%s\n", pline); }

                store_line(h, pline); // guardar en el historial

                bg = check_bg(pline); // ver si tiene un & al final

                command* cmds = split_commands(pline); // divir los comandos por los pipes |

                int fd = -1;
                while(cmds->id) { cmds->bg = bg, fd = execute(cmds++, fd); } // ejecutar cada uno de ellos


                while(!empty(childs)) { // esperar por cada uno de izquierda a derecha
                    waitpid(top(childs), &last_status, 0);
                    if(!empty(childs)) { pop(childs); }

                    if(!WIFEXITED(last_status) && !WIFSIGNALED(last_status)){ perror("MasterShell"); } // imprime el error del comando correspondiente
                    else if(WEXITSTATUS(last_status) == 2) { printf("MasterShell: command not found\n"); } // no existe el comando
                }
            }
            else { printf("MasterShell: !%s: event not found\n", herror); }
            free(herror), free(pline);
        }
        free(sline), free(rline);
    }
}

void init() {
    user = getlogin(); // inicializar el buffer de usuario
    hostname = malloc(ht_buffer * sizeof(char)); // inicializa el buffer de hostname
    wd = malloc(wd_buffer * sizeof(char));  // inicializar el directorio con tamaño wd_buffer definido en .config.h
    gethostname(hostname, ht_buffer);

    line = malloc(ln_buffer * sizeof(char)); // inicializar la linea a parsear con tamaño ln_buffer definido en .config.h

    h = open_history(hst_log, 50); // abre el historial

    childs = init_queue(); // crea la pila de los hijos
    signal(SIGINT, handler); // cambiar el handler para la senhal de interrupcion
    signal(SIGCHLD, Handler); // handler de SIGCHLD para procesos en background
    
    welcome();
}

void welcome() {
    system("clear");
    printf(TOOLS"               @@@                                            @@@@@@@           \n");
    printf("         @@@   @..@                                        @@@.....  @@@        \n");
    printf("         @..@   @..@                                      @..........   @@      \n");
    printf("          @..@   @..@        Welcome to MasterShell      @..............  @     \n");
    printf("           @..@ @...@                                    @..............  @     \n");
    printf("      @@@   @..@...@       Press <enter> to continue.     @............  @      \n");
    printf("      @..@   @....@                                        @..........  @       \n");
    printf("       @..@ @......@              "RESET SHELL"*&@@&@@@@&,"RESET TOOLS"               @.......@@@@        \n");
    printf("        @..@...@....@           "RESET SHELL"@@,         *@#"RESET TOOLS"            @....@@@@            \n");
    printf("         @....@ @....@ "RESET SHELL"(@@@@@@(.@/             (@.#@@@@@&/"RESET TOOLS"@....@                \n");
    printf("          @..@   @ @"RESET SHELL"@@      ..&@               &@..     .&@"RESET TOOLS".@ @                 \n");
    printf("           @@     "RESET SHELL"(@.       ..@*               (@..       .@*"RESET TOOLS"@                  \n");
    printf(RESET SHELL"                 .@.        ..@*               /@..        ,@                   \n");
    printf("                 (&         ..@*               (&..         @*                  \n");
    printf("                 #&         ..                 &@..         @/                  \n");
    printf("              .,.*@         ..(@               @/..        .@,.,.               \n");
    printf("           @@@//(&@*         .,@.             ,@..         /@&(//@@@            \n");
    printf("         /@.   ...(@         ..&(             @&..         @/...   ,@*          \n");
    printf("        ,@.     ...@#         ./@            .@*.         @&...     .@.         \n");
    printf("        #&       ...@*        ..@*           (@..        (@...       &/         \n");
    printf("        *@        ..,@,        .(&           @/.        *@,..        @,         \n");
    printf("         @         ..*@,        .@*         /@.        *@,..        @@          \n");
    printf("          @#        ..*@,       .#&         @/.       *@,..        @&           \n");
    printf("           @@        ..,@*       .@,       /@.       (@...       .@(            \n");
    printf("          (&@@/       ...@&      .#&       @(.      @@..        #@@&/           \n");
    printf("        (@....#@,       ..#@.     ,@,     /@.     .@/..       *@(.. ,@*         \n");
    printf("        @*   ...@@.      ..*@*     @&     @(     /@,.       ,@@..    /@         \n");
    printf("        @&      ..@@,      ..@#    .@,   *@.    @@..      *@@..      @(         \n");
    printf("         #@*      ..#@/      .@@    @&   @(   .@#.      (@(..      /@/          \n");
    printf("           *@@/      ./@@.    .*@*  ,@. *@.  /@,     .&@*.      (@&,            \n");
    printf("              *@@#.    .*@@/    ,@(   @@  @@.    /@@,.    ,@@&*                 \n");
    printf("                  ,@@@#.   *@@*   @@.*@.@,.@(   /@@,   ,#@@#.                   \n");
    printf("                       @&@@@#,*@@( ,@#@@&@@..#@&,*@@@@&@                        \n");
    printf("                      ,@.    ./&@@@@@@@@@@@@@@@@/..   ,@.                       \n");
    printf("                        #@@@@@@@@@@@@&@(@@@@@@@@@@@@@@#                         \n");
    printf(RESET TOOLS"                             @....@         @....@                              \n");
    printf("                            @....@           @....@                             \n");
    printf("                           @....@             @....@                            \n");
    printf("                            @@@@               @@@@                             \n");
    printf("                                                                                \n");
    printf("                           Roberto Marti Cedeño C312                            \n");
    printf("                        Daniel Alberto Garcia Perez C312                        \n"RESET);
    getline(&line, &getln_a, stdin);
    system("clear");
}

void quit(int status) {
    system("clear");

    free(hostname), free(wd), free(line);
    close_history(h), del_queue(childs);
     
    exit(status);
}

int execute(command* cmd, int input_fd) { //ejecutar un comando, si no es final retorna un fd con la salida 
	int p[2];
	pipe(p);

    if(!strcmp(cmd->id, "cd")) { //si es cd
		close(p[1]);
        if(cmd->args != 2) { printf("MasterShell: cd: too many arguments\n"); } // si existen mas de 2 argumentos imprimir error
        else if(cmd->start && cmd->final && chdir(cmd->argv[1])) { printf("MasterShell: cd: "); fflush(stdout); perror(cmd->argv[1]); } // si el chdir retorna error
    }
    else if(!strcmp(cmd->id, "jobs"))
    {
        close(p[1]);
        if(cmd->args != 1) { printf("MasterShell: jobs: too many arguments\n"); }
        else { bkprint(&back); }
    }
    else if(!strcmp(cmd->id, "fg"))
    {
        close(p[1]);
        if(cmd->args != 2) { printf("MasterShell: fg: too many arguments\n"); }
        else
        {
            if(!IsNumber(cmd->argv[1]))
            {
                backgpid = bkseek(atoi(cmd->argv[1]),&back);
            }
            else
            {
                backgpid = bkseekbyname(cmd->argv[1],&back);
            }
            if(backgpid == -1){ printf("MasterShell: fg: command not found\n"); }
            else
            {
                int number = bkseeknumber(backgpid,&back);
                int cont = back.count[number];
                bksilentdelete(backgpid,&back);
                siginfo_t sig;
                do
                {
                    waitid(P_PGID, backgpid, &sig, WEXITED);
                }
                while(--cont && !finish);
            }
        }
    }
    else if(!strcmp(cmd->id, "exit")) { //si se escribe el comando exit salir de la consola
    	close(p[1]);
    	if(cmd->start && cmd->final) {
        	close(p[0]);
            quit(last_status);
        }
    }
    else {
        int pid = fork();

	    if (!pid) { // el hijo se encarga de ejecutar el comando
            if (bg) 
            {
                if(gpid == -1) 
                {
                    setpgid(0,0);
                    gpid = pid;
                }
                else
                {
                    setpgid(pid,gpid);
                }
            }
			close(p[0]);

    		char** params = calloc((cmd->args + 1) * sizeof(char*), sizeof(char*));
    		int i, j, fdin = input_fd, fdout = p[1];
    		if(cmd->final) { close(p[1]); fdout = -1; }

    		for(i = 0, j = 0; i < cmd->args; ++i) {
				if(!strcmp(cmd->argv[i], ">")) {
					close(fdout);
					if ((fdout = open(cmd->argv[++i], O_WRONLY | O_TRUNC)) < 0 ) { 
                        fdout = open(cmd->argv[i], O_WRONLY | O_TRUNC | O_CREAT, 0644); 
                    }
				}
				else if(!strcmp(cmd->argv[i], "<")) {
					close(fdin);
					if((fdin = open(cmd->argv[++i], O_RDONLY)) < 0) { perror("MasterShell"); exit(0); }
				}
				else if(!strcmp(cmd->argv[i], ">>")) {
					close(fdout);
					if ((fdout = open(cmd->argv[++i], O_WRONLY | O_APPEND)) < 0 ) { 
                        fdout = open(cmd->argv[i], O_WRONLY | O_CREAT, 0644);
                    }
				}
				else { params[j++] = cmd->argv[i]; }   
    		} 
			params[j] = NULL;

			dup2(fdin, 0);
			close(fdin);

			dup2(fdout, 1);
	    	close(fdout);

            if(!strcmp(params[0], "history")) { show_history(h); exit(0); }

    		execvp(params[0], params);
            if(bg)
            {
                deleteall(pid, &back);
            }
    		exit(2);
	    }
	    else {
            if(cmd->bg)
            {
                if(pgpid == -1) 
                {
                    setpgid(pid,pid);
                    pgpid = pid;
                    bkinsert(pid,pid,rline,&back);
                }
                else
                {
                    setpgid(pid,pgpid);
                    bkinsert(pid,pgpid,rline,&back);
                }
            }
            //if(!cmd->bg)
            else { push(childs, pid); }
			close(p[1]);
        }
    }

	return p[0];
}

int check_bg(char* line) {
    int i;
    for(i = strlen(line) - 1; i >= 0; --i) {
        if(line[i] != ' ' && line[i] != '\n') {
            if(line[i] == '&') {
                line[i] = ' ';
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

void handler(int sig) {
	if(!empty(childs)) { 
		printf("\n"); 
		while(!empty(childs)) { kill(pop(childs), sig); } 
	}
	if(backgpid != -1)
    {
        killpg(backgpid, sig);
    }
}

int IsNumber(char* s)
{
    char* a = s;
    while(*a)
    {
        if(*a < 48 || *a > 57)
        {
            return 1;
        }
        ++a;
    }
    return 0;
}
