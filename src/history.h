#if !defined hhistory
#define hhistory

typedef struct hst {
	char* file;
	char** lines;
	int count;
	unsigned int buff_size;
} history;

history* open_history(char*, unsigned int);

void close_history(history*);

void show_history(history*);

void store_line(history*, char*);

char* load_line(history*, char*);

char* load_last_line(history*);

char* process_line(history*, char*, char**);

#endif
