#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int size;

char* line;
char** split(char *, const char *);

void ffree(char**);

int main(int args, const char** argv)
{
    line = malloc(150 * sizeof(char));
    int a = 0;
    printf("$:");
    getline(&line,&a,stdin);
    char** test = split(line, " ");
    for(int i = 0; i < size; ++i)
    {
        printf("%s\n", test[i]);
    }
    execvp(test[0],test);
    ffree(test);
    free(line);
}

char** split(char* string, const char* pattr)
{
    char** result = calloc(10 *sizeof(char*),sizeof(char*));
    result[0] = calloc(50*sizeof(char),sizeof(char));
    int cond = 0;
    int start = 0;
    int size1 = 10;
    int size2 = 50;
    int j = 0;
    
    for(int i = 0; *string; ++string)
    {
        if(i == size2)
        {
            result[i] = realloc(result[i] , (size2 + 50) * sizeof(char));
            size2 += 50;
        }
        if(*string == *pattr)
        {
            if(!cond && start)
            {
                if(j == size1)
                { 
                    result = realloc(result, (10 + size1)* sizeof(char*));
                    size1 += 10;
                }
                ++j;
                result[j] = calloc(50*sizeof(char),sizeof(char));
                cond = 1;
            }
            i = 0;
        }
        else
        {
            start = 1;
            printf("%d\n", *string);
//             if(*string == 92)
//             {
//                 ++string;
//                 if(*string == 110) {continue;}
//                 --string;
//             }
            if(*string != 10)
            {
                result[j][i] = *string;
                ++i;
                cond = 0;
            }
        }
    }
    size = j + 1;
    return result;
}

void ffree(char** sub)
{
    for(int i = 0; i < size; ++i)
    {
        free(sub[i]);
    }
    free(sub);
}
 
