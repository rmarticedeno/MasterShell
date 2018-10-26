#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include ".config.h"

int size; //tamaño de cada línea a parsear empezando en 1
char* line; // línea a parsear
char* hostname; //nombre de la maquina
char* user; //nombre de usuario

char* wd; //directorio actual de trabajo

char** split(char *, const char *); //divide un string por un delimitador (repetido n veces n>=1)
void fforf(int, char**); //ejecuta execvp
void execute(int, char**); //dada una linea parseada ejecuta el comando correspondiente
void ffree(char**); //libera la memoria de char**
void init(); //inicializa todos los buffers

int main(int args, const char** argv)
{
    init();
    while(1)
    {
        getwd(wd); // obtener el directorio actual
        printf(format, user, hostname, wd); // format definido en .config.h
        getline(&line,&getln_a,stdin);
        char** test = split(line, " ");
        execute(size, test);
        ffree(test);
        free(line);
    }
}

void init()
{
    user = getlogin(); // inicializar el buffer de usuario
    hostname = malloc(ht_buffer * sizeof(char)); // inicializa el buffer de hostname
    line = malloc(ln_buffer * sizeof(char)); //inicializar la linea a parsear con tamaño ln_buffer definido en .config.h
    wd = malloc(wd_buffer * sizeof(char));  //inicializar el directorio con tamaño wd_buffer definido en .config.h
    gethostname(hostname,ht_buffer);
}

char** split(char* string, const char* pattr)
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
                splitted[j] = calloc(50*sizeof(char),sizeof(char));
                cond = 1;
            }
            i = 0; //reiniciar contador
        }
        else
        {
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
    size = j + 1; // total de divisiones empezando por 1
    return splitted;
}

void ffree(char** sub) //vaciar un buffer
{
    for(int i = 0; i < size; ++i)
    {
        free(sub[i]);
    }
    free(sub);
}

void execute(int args, char** argv) //ejecutar un comando
{
    if(!strcmp(argv[0], "cd")) //si es cd
    {
        if(args != 2) // si existen mas de 2 argumentos imprimir error
        {
            printf("bash: cd: too many arguments\n");
            return;
        }
        else
        {
            if(chdir(argv[1])) // si el chdir retorna error
            {
                perror("cd"); //magia :° imprime el mensaje del error con el encabezado que se le pase(cualquier error) :-!
            }
//             else
//             {
//                 printf("%s\n",getwd(wd));  // descomentar para imprimir el direcorio actual de trabajo
//             }
        }
    }
    else if(!strcmp(argv[0], "exit")) //si se escribe el comando exit salir de la consola
    {
        free(line);
        free(wd);
        free(hostname);
        exit(0);
    }
    else
    {
        ffork(args, argv); //ejecutar el comando correspondiente con execvp
    }
}

void ffork(int args, char** argv)
{
   int pid = fork();
    if (!pid)
    {
        char** par = malloc((args) * sizeof(char));
        int i;
        for(i = 0; i <= args; ++i)
        {
            if (i != args)
            {
                par[i] = argv[i];
            }
            else
            {
                par[i] = NULL;
            }
        }
        execvp(argv[0],par);
    }
    else
    {
        int status;
        wait(&status);
        if(!WIFEXITED(status))
        {
            perror(argv[0]); // imprime el error del comando correspondiente
        }
    }
}

