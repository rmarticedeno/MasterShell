#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

queue* init_queue() {
	queue* q = malloc(sizeof(queue*));
	q->buff_size = 8;
	q->begin = q->end = 0;
	q->elems = malloc(q->buff_size * sizeof(T*));

	return q;
}

void del_queue(queue* q) { free(q->elems); }

void push(queue* q, T elem) {	
	q->elems[q->end++] = elem;
	if(q->end == q->buff_size) {
		q->buff_size += 8;
		q->elems = realloc(q->elems, q->buff_size * sizeof(T*));
	}	 
}

T top(queue* q) { return q->elems[q->begin]; }

T pop(queue* q) { return q->elems[q->begin++]; }

int empty(queue* q) { 
	if(q->end <= q->begin) {
		q->begin = q->end = 0;
		return 1;
	}
	return 0;
}

void print_queue(queue* q) {
	printf("Elements: \n--- ");
	int i;
	for(i = q->begin; i < q->end; ++i) {
		printf("%d ", q->elems[i]);
	}
	printf("\n");
}