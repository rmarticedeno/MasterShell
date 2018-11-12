#if !defined hqueue
#define hqueue

typedef int T;

typedef struct stk {
	T* elems;
	int begin;
	int end;
	int buff_size;
} queue;

queue* init_queue();

void del_queue(queue*);

void push(queue*, T);

T top(queue*);

T pop(queue*);

int empty(queue*);

void print_queue(queue*);

#endif
