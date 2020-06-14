/*
 General thread related declarations put here to declutter 
 the scheduler file for readability.
 */

#ifndef THREAD_INFO
#define THREAD_INFO

/*
 Details each lift request and the movement of each
 one before and after completion.
 */
typedef struct thread_info {
    int tid;
    int prev;
    int request_mov;//dequeue thing and insert
    int request_total;
    int movement_total;
    int curr_pos;//curr_pos is request_mov after
    
} thread_info;

//makes logging more readable
#define INTRO_LINE "\nLift-%d Operation\nPrevious Position: Floor %d\nRequest: Floor %d to Floor %d\n"
#define DETAIL_LINE "Detail Operations:\n \tGo from Floor %d to Floor %d\n \tGo from Floor %d to Floor %d\n"
#define SUMMARY_LINE "\t#Movement for this request: %d\n\t#request: %d\n \tTotal #movement: %d\n Current position: Floor %d\n"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//buffer access
pthread_mutex_t shared_vars = PTHREAD_MUTEX_INITIALIZER;//thread sync mutex i.e. num task counter

//block if buffer is full, signal if queue has size-1 elements or less--> consumer go --> until not full
pthread_cond_t until_not_full = PTHREAD_COND_INITIALIZER;

//block if buffer is empty, signal if at least 1 element in queue --> producer go --> until not empty
pthread_cond_t until_not_empty = PTHREAD_COND_INITIALIZER;

#endif
