#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "history.h"

int cnt_digits(int n) {
	int cnt = 0;
	while(n) { 
        ++cnt;
        n /= 10;
    }
	return cnt;
};

char* itoa(int n) { // si n > 0
    int d = cnt_digits(n);

    char* num = calloc((d + 1) * sizeof(char), sizeof(char));
    
    int i;
    for(i = d - 1; i >= 0; --i) {
        num[i] = n % 10 + '0';
        n /= 10;
    }

    return num;
};

history* open_history(char* file, unsigned int max_size) {
	history* h = malloc(sizeof(history));
	h->file = file;
	h->lines = malloc(max_size * sizeof(char*));
	h->count = -1;
	h->buff_size = max_size;

	int fd = open(file, O_RDONLY);
	if(fd == -1) { fd = open(file, O_CREAT | O_RDONLY, S_IRWXU); }

	int index = 0, size = 8;
	char* line =  malloc(size * sizeof(char));

	char c, clast = '\n';
	while(read(fd, &c, 1)) {
		if(index == size) {
			size += 8;
			line = realloc(line, size * sizeof(char));
		}
		
		line[index++] = c;
        
        if(c == '\n') {
            if(clast == '\n') { continue; }
            line = realloc(line, index * sizeof(char));
            line[index - 1] = 0;
            
			size = index;
			index = 0;

			store_line(h, line);
        }
        
        clast = c;
	}
	if(clast != '\n') { 
		line = realloc(line, index * sizeof(char));
        line[index] = 0;
        store_line(h, line);
	}

	free(line);
	
	close(fd);
	
	return h;
}

void close_history(history* h) {
	int fd = open(h->file, O_WRONLY | O_TRUNC);
	if(fd == -1) { fd = open(h->file, O_WRONLY | O_CREAT, 0644); }
	
	int i;
	for(i = 0; i <= h->count; ++i) {
		write(fd, h->lines[i], strlen(h->lines[i]));
		write(fd, "\n", 1);
		free(h->lines[i]);
	}
	
	close(fd);
	free(h);
}

void show_history(history* h) {
	int tab = cnt_digits(h->count + 1);
	
	int i;
	for(i = 0; i <= h->count; ++i) {
		int spc = tab - cnt_digits(i + 1);
		printf("  ");
		while(spc--) { printf(" "); } // boberias xddddd
		printf("%d  %s\n", i + 1, h->lines[i]);
	}	
}

void store_line(history* h, char* line) {
	int n = strlen(line);

	if(!n || line[0] == ' ') { return; } // no puede empezar con espacios
	if(h->count >= 0 && !strcmp(h->lines[h->count], line)) { return; } // no repetir
	
	if(h->count == h->buff_size - 1) { // si esta lleno desplaza los punteros
		char* first = h->lines[0];
		int i;
		for(i = 0; i < h->count; ++i) { 
			h->lines[i] = h->lines[i + 1];
		}
		h->lines[h->count] = first;
		free(first);
	}
	else { ++h->count; }

	h->lines[h->count] = calloc((n + 1) * sizeof(char), sizeof(char));
	
	h->lines[h->count] = strncpy(h->lines[h->count], line, n);
}

char* load_line(history* h, char* start) {
	int i, n = strlen(start);
	for(i = h->count; i >= 0; --i) {
        if(!strcmp(itoa(i + 1), start)) { return h->lines[i]; } // caso del numero de linea
		if(!strncmp(h->lines[i], start, n) && (h->lines[i][n] == ' ' || h->lines[i][n] == '\n')) { 
			return h->lines[i];
		} // caso del inicio de linea
	}
	return 0;
}

char* load_last_line(history* h) { return load_line(h, itoa(h->count + 1)); }

char* process_line(history* h, char* line, char** arg) {
    int n = strlen(line);
    int size = n + 1;
    char* new_line = calloc(size * sizeof(char), sizeof(char));
    
    int i = 0, j = 0;
    while(i < n) {
        char c = line[i++];
        
        if(c == '!') {
            arg[0] = calloc(n * sizeof(char), sizeof(char));
           
            int k = 0;
            while(i < n && line[i] && line[i] != ' ') { arg[0][k++] = line[i++]; }
            
            if(k) {
                char* entry;

                if(!strcmp(arg[0], "!")) { entry = load_last_line(h); }
                else { entry = load_line(h, arg[0]); }
                
                if(!entry) { return new_line; }

                int ne = strlen(entry);
                size += ne;
                new_line = realloc(new_line, size * sizeof(char));
                
                k = 0;
                while(k < ne) { new_line[j++] = entry[k++]; }
            }
            else { new_line[j++] = c; }
           
            free(arg[0]);
        }
        else { new_line[j++] = c; }
    }
    new_line[j] = 0;
    
    arg[0] = calloc(sizeof(char), sizeof(char));
    return new_line;
}
