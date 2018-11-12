#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include "background.h"
#include <stdio.h>
#include <errno.h>

void bkprint(struct bgl* input)
{
    for(int i = 0 ; i < bg_buffer; ++i)
    {
        if(input->used[i])
        {
            printf("[%d]: %d, %s\n", i, input->pid[i], input->name[i]);
        }
    }
}

int bkinsert(int pid, int pgid , char* name,struct bgl* input)
{
    for(int i = 0 ; i < bg_buffer; ++i)
    {
        if(input->gpid[i] == pgid && input->used[i])
        {
            input->pid[i] = pid;
            ++input->count[i];
            input->maxcount[i] = input->count[i];
            return 0;
        }
    }
    for(int i = 0; i < bg_buffer; ++i)
    {
        if(!input->used[i])
        {
            input -> gpid[i] = pgid;
            input -> used[i] = 1;
            input -> pid[i] = pid;
            input -> name[i] = malloc(strlen(name)* sizeof(char));
            strcpy(input->name[i], name);
            ++input -> count[i];
            input->maxcount[i] = input->count[i];
            printf("[%d]: %s\n", i, input->name[i]);
            return 0;
        }
    }
    return -1;
}

int bkdelete(int pid,struct bgl* input, int message)
{
    for(int i = 0 ; i < bg_buffer; ++i)
    {
        if(input -> used[i] && (pid <= input->pid[i]) && (pid - 1 >= (input->pid[i] - input->maxcount[i])))
        {
            if(input->count[i] > 1)
            {
                --input->count[i];
                return input->count[i];
            }
            else
            {
                if(message)
                {
                    printf("[%d]: %d %s\n",i,input -> pid[i], input -> name[i]);
                }
                input->used[i] = 0;
                input->count[i] = 0;
                return 0;
            }
        }
    }
    return -1;
}

int bksilentdelete(int gpid,struct bgl* input)
{
    for(int i = 0 ; i < bg_buffer; ++i)
    {
        if(input -> used[i] && input->gpid[i]==gpid)
        {
            input->used[i] = 0;
            input->count[i] = 0;
            return 0;
        }
    }
    return -1;
}

int deleteall(int pid, struct bgl* input) //used only if exec fails
{
    for(int i = 0; i < bg_buffer; ++i)
    {
        if(input->used[i] && input->pid[i] == pid)
        {
            input->used[i] = 0;
            input->count[i] = 0;
            return 0;
        }
    }
    return -1;
}

int bkdeletebyname(char* name,struct bgl* input)
{
    for(int i = 0 ; i < bg_buffer; ++i)
    {
        if(input -> used[i] && !strcmp(name,input -> name[i]))
        {
            printf("[%d]: %d %s\n",i,input -> pid[i], input -> name[i]);
            input->used[i] = 0;
            input->count[i] = 0;
            return 0;
        }
    }
    return -1;
}

int bkseek(int number,struct bgl* input)
{
    if(input -> used[number])
    {
        return input->gpid[number];
    }
    return -1;
}

int bkseeknumber(int gpid,struct bgl* input)
{
    for(int i = 0; i < bg_buffer; ++i)
    {
        if(input -> used[i] && input-> gpid[i] == gpid)
        {
            return i;
        }
    }
    return -1;
}

int bkseekbyname(char* name,struct bgl* input) // para ver si se encuantra un string
{
    for(int i = bg_buffer - 1 ; i >= 0; --i)
    {
        if(input -> used[i] && substring(name,input->name[i]))
        {
            return input->gpid[i];
        }
    }
    return -1;
}

void Signal(int signum, void* handler1, int option)
{
    struct sigaction sig, old_action;
    sigemptyset(&sig.sa_mask);
    
    if(!option)
    {
        sig.sa_sigaction = handler1;
        sig.sa_flags = SA_SIGINFO;
    }
    else
    {
        sig.sa_flags = SA_RESTART;
        sig.sa_handler = handler1;
    }
    
    if(sigaction(signum, &sig, &old_action) < 0)
    {
        perror("Signal Error");
    }
    
    return;
}

void SIGCHLD_Handler(int signal, siginfo_t* info, void* ucontext)
{
    if(signal == SIGCHLD)
    {
        int newpid = (int)info->si_pid;
        bkdelete(newpid, &back, 1);
    }
}

void Handler(int signal)
{
    int status;
    int otro = waitpid(-1, &status, WNOHANG);
    int aux = bkdelete(otro, &back,1);
    while(otro != 0 && otro != -1)
    {
        otro = waitpid(-1, &status, WNOHANG);
        bkdelete(otro, &back,1);
    }
}

int substring(char* s1, char* s2)
{
    int i, j, ok;
    char *a = s1;
    char *b = s2;
    while(*b != 0)  //mientras la posicion a a que apunta el puntero no sea el fin de cadena
    {
        ok = 1;
        char *u = b, *v = a;
        for(; *v != 0 && *u != 0; ++u)
        {
            ok &= (*v == *u); //and between ok = 1 and el valor del char en ambas cadenas
            ++v;
            if(ok && *v == 0)
            {
     //           printf("1\n");
                return 1;
            }
        }
        ++b;
    }
   // printf("0\n");
    return 0;
};
