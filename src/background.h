#if !defined background
#define background

#define bg_buffer 20
//lista de los background
struct bgl
{
    int maxcount[bg_buffer]; //maxcount of each family of process
    int gpid[bg_buffer]; //gpid of each process group
    int used[bg_buffer]; //boolean array de usados
    int count[bg_buffer]; //Cantidad de procesos en el pipeline
    int pid[bg_buffer]; //pid of each child process in background
    char* name[bg_buffer]; // name of the childs process 
} back;
void bkprint(struct bgl*);
int bkinsert(int, int, char*,struct bgl*);
int bkdelete(int,struct bgl*, int);
int bkdeletebyname(char*,struct bgl*);
int bkseek(int,struct bgl*);
int bkseekbyname(char*,struct bgl*);
int bkseeknumber(int,struct bgl*);
int bksilentdelete(int,struct bgl*);
int deleteall(int, struct bgl*);
void Signal(int, void*, int);
void SIGCHLD_Handler(int, siginfo_t*, void*);
int substring(char*, char*);
void Handler(int);
char* Debackground(char*);
//void initialize(struct bgl*);
//void destroy(struct bgl*);
#endif
