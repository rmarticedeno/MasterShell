#if !defined config
#define config

#define hst_log "history.txt"
#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define NEGRITA "\x1b[1m"
#define PROMPTUSER GREEN NEGRITA 
#define PROMPTPATH YELLOW NEGRITA
#define SHELL BLACK NEGRITA
#define TOOLS CYAN NEGRITA

int wd_buffer = 500;
int ln_buffer = 200;
int us_buffer = 100;
int ht_buffer = 100;
char* format = PROMPTUSER "%s@%s" RESET ":" PROMPTPATH "%s"RESET"$ ";
long unsigned int getln_a = 0;

#endif
