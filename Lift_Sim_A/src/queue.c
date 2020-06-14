/* FILE:        queue.c
 * AUTHOR:      Andrew Li
 * DESCRIPTION: Non-generic queue implementation
 * REFERENCES:  Advanced Data Structures - Peter Brass
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Initializer for queue structure
 * Allocates new queue structure with task array data
 * Sets up capacity for empty/non-empty conditions and pointers
 * Params: Size of buffer from cmd args.
 * Exports: initialised queue.
 */
queue *init_queue(size_t buf_size)
{
    queue *q = malloc(sizeof(queue));
    q->tasks = malloc(sizeof(buf_size));
    q->size = 0;
    q->front = q->size;
    q->rear = buf_size - 1;
    q->capacity = buf_size;
  
    return q;
}

/* Adds a new struct of task* to the end of the queue i.e.
 * the last item of the queue in a FIFO queue.
 * Params: queue and task to add to queue
 * Exports: Error code.
 */
void enqueue(queue *q, int in_src, int in_dest)
{
    task new_task;//make new node
    new_task.src = in_src;
    new_task.dest = in_dest;

    q->rear = (q->rear + 1) % q->capacity;//update ptr
    q->tasks[q->rear] = new_task;//attach to butt
    q->size = q->size + 1;
}

/* Takes off task struct at top of queue.
 * Params: Queue to be dequeued
 * Exports: Task that was dequeued.
 */
task *dequeue(queue *q)
{
    task *t = &q->tasks[q->front];//get first task
    q->front = (q->front + 1) % q->capacity;//update ptr
    q->size = q->size - 1;
    return t;
}

int is_full(queue *q)
{
    return (q->size == q->capacity);			       
}

int is_empty(queue *q)
{
    return (q->size == 0);
}

void remove_queue(queue *q)
{
    free(q->tasks);
    free(q);
}

