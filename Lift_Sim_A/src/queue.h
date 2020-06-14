#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

/* FILE: queue.h*/
/* AUTHOR: Andrew Li*/
/* simple queue structure implementation */
/* stores task information to be threaded */
/* front and rear pointers to keep track, */
/* as well as number of elements stored */
/* TODO: add thread-safety to structure itself */


/* Containter of lift request info 
 *
 */
typedef struct task {
    int src;
    int dest;
} task;


typedef struct queue{
    int front, rear;//pointers to position
    int capacity;//buffer size
    int size;//num_elements
    task *tasks;//contents
    
} queue;

queue *init_queue(size_t capacity);

void enqueue(queue *q, int, int);

task *dequeue(queue *q); 

int is_full(queue* q);

int is_empty(queue* q);

void remove_queue(queue *q);

#endif
