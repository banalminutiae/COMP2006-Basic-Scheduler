/* AUTHOR: Andrew Li - 19460677 
* DATE CREATED: 26/03/20 
* FILE: main.c 
* PURPOSE: Preliminary file opening and checking, as well  
* as resource allocation for threads, logging and an implementation. 
* of the consumer-producer threads and a solution to the bounded buffer problem.
* Utilises POSIX threads to synchronise threads and implement bounded buffer problem */


//ISSUES: once the first task is dequeued, the producer locks endlessly
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>

#include "thread_info.h"
#include "queue.h"
#include "Logger.h"

const unsigned int THREAD_COUNT = 3;
const char *INPUT_FILE_PATH = "sim_input";

queue *q;//shared bounded buffer
Logger *logger;//thread safe logging

//cmd args
int sleep_time = 0; 
int buf_size = 0;

int num_tasks = 0;//essentially num_elements
int total_tasks = 0;//number of tasks read and processed so far
int EOD = 0;//end of data, signal that reading is done

void *lift(void *param);
void *request();


/* FUNCTION: main
 * PARAM:    command line args
 * OUT:      NULL
 * DESC:     declares and initialises consumer and producer threads, mutexes
 *           and mutex conditions. Performs cleanup after completion and prints final
 *           result.
 */
int main(int argc, char **argv)
{
    pthread_t lift_thread[THREAD_COUNT];//consumer threads id
    pthread_t lift_request;//producer thread id
    char final_result[256];

    if (argc != 3) {
	fprintf(stderr, "Enter buffer size and sleep value.\n");
	return -1;
    }
    
    if (atoi(argv[1]) <= 0 || atoi(argv[2]) < 0) {
	fprintf(stderr, "Invalid\n");
	return -1;
    }
    
    buf_size = atoi(argv[1]);
    sleep_time = atoi(argv[2]);

    q = init_queue(buf_size);
    
    logger = malloc(sizeof(Logger));
    init_log(logger);

    pthread_create(&lift_request, NULL, request, NULL);
    
    //3 consumer threads and the thread_info array
    thread_info **info = malloc(sizeof(thread_info*) * THREAD_COUNT);
    for (int ii = 0; ii < THREAD_COUNT; ii++) {	
	info[ii] = malloc(sizeof(thread_info));
	info[ii]->tid = (ii + 1);//assign ID
	pthread_create(&lift_thread[ii], NULL, lift, (void*)info[ii]);//activate consumer threads
    } 

    //cleanup
    pthread_cond_broadcast(&until_not_full);//awaken consumer threads to finish up
    
    //finish up lifts, print finale	  
    for(int ii = 0; ii < THREAD_COUNT; ++ii) {
		pthread_join(lift_thread[ii], NULL);
		printf("Lift thread ended\n");
    }
    pthread_join(lift_request, NULL);

    pthread_mutex_destroy(&mutex);

     //finishing up
    sprintf(final_result, "Total number of requests: %d\n Total number of movements: %d\n", total_tasks, 1); 
    write_log(logger, final_result);
  
    remove_queue(q);

    fclose(logger->sim_out);
    free(logger);
     
    return 0;
}

/* FUNCTION: request()
 *----------------------
 * PARAM: NULL
 * OUT:   NULL
 * DESC:  Producer function in bounded buffer problem. Checks and 
 *        reads in a lift request line
 */
void *request()
{
    FILE *sim_input = NULL;
    int in_src, in_dest;
    char log_line[256];

    sim_input = fopen(INPUT_FILE_PATH, "r");
    if(sim_input == NULL) {
	fprintf(stderr, "error reading file\n");
    }

    while (!feof(sim_input)) {
	printf("locking producer\n");
        pthread_mutex_lock(&mutex);//start critical section
	//wake up consumers if buffer has task
	//block if the buffer is full
	if (q->size > 0) {
	    printf("broadcast to consumers\n");
	    pthread_cond_broadcast(&until_not_empty);
	    while (is_full(q)) {
		printf("waiting since full\n");
		pthread_cond_wait(&until_not_full, &mutex);//wait till until_not_full signalled
	    }
	}

        if (fscanf(sim_input, "%d %d", &in_src, &in_dest) == 2) {
	    if (in_src > 0 && in_src > 0) {//skips bad lines
		printf("read line\n");
		
		enqueue(q, in_src, in_dest);
		printf("enqueued %d %d\n", in_src, in_dest);

		//updating counter
		num_tasks++;
		total_tasks++;

		printf("new task logged\n");
		sprintf(log_line, "\nNew Lift Request From Floor %d to Floor %d\n Request No: %d\n",
			in_src, in_dest, total_tasks);

		write_log(logger, log_line);
		write_log(logger, "\n---------------------------------\n");

		pthread_cond_broadcast(&until_not_full);//all consumers stop blocking on empty cond, get eating
	    }
	    
	    printf("unlocking producer\n");
	}
	pthread_mutex_unlock(&mutex);//leave critical section
    }
    pthread_mutex_lock(&shared_vars);
    EOD = 1;
    pthread_mutex_unlock(&shared_vars);
    pthread_cond_broadcast(&until_not_full);//finish off the rest
    fclose(sim_input);
    return NULL;
}


/*
 * FUNCTION: lift()
 * -----------------
 * PARAM: void pointer to thread information
 * OUT:   NULL
 * DESC:  Consumer function in bounded buffer problem. Dequeues off
          shared buffer and logs the description of the action taken.
 */
void *lift(void *param)
{    
    thread_info *thread = (thread_info*)param;
    thread->request_total = 0;
    
    //logging lines
    char intro_line[256];
    char detail_line[256];
    char summary_line[256];
    task *tsk;
    
    while(1) {
	
        pthread_mutex_lock(&mutex);//enter critical section
        printf("lock consumer\n");

	printf("dequeueing\n");
	tsk = dequeue(q);
	 
	//wake up producer to add more
	//block if the buffer is empty
	//pthread_mutex_lock(&shared_vars);
	if (q->size <= 0 && !EOD) {
	    printf("signal to add\n");
	    pthread_cond_broadcast(&until_not_full);

	    //counter instead of queue method bc edge cases
	    while(q->size <= 0 && !EOD) {
		printf("waiting since empty\n");
		pthread_cond_wait(&until_not_empty, &mutex);
	    }
        }
	//pthread_mutex_unlock(&shared_vars);

	thread->request_total++;
	
	num_tasks--;
	
	printf("Dequeued task src: %d\n", tsk->src);
	printf("Dequeue task dest: %d\n", tsk->dest);
	
	thread->request_mov = abs((thread->prev - tsk->src) + (tsk->dest - tsk->src));

	printf("Request consumed\n");
	thread->movement_total += thread->request_mov;

	thread->curr_pos = tsk->src;
	thread->prev = tsk->src;
	
	printf("logging deets\n");
	sprintf(intro_line, INTRO_LINE, thread->tid, thread->prev, tsk->src, tsk->dest);
	write_log(logger, intro_line);
	
	sprintf(detail_line, DETAIL_LINE, thread->curr_pos, tsk->src, tsk->src, tsk->dest);

	write_log(logger, detail_line);

	sprintf(summary_line, SUMMARY_LINE, thread->request_mov, thread->request_total, thread->movement_total, tsk->dest);
	write_log(logger, summary_line);
	
	thread->curr_pos = tsk->dest;//update lift position
	sleep(sleep_time);

	//let producer add to buffer since no longer full
	printf("get adding\n");
	///////////
	pthread_cond_signal(&until_not_empty);
	///// broken!
	
	printf("unlock consumer\n");
	pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}
