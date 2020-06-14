#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"

//Initialises thread safe logger
void init_log(Logger *logger)
{
    pthread_mutex_init(&logger->mutex_log, NULL);
    logger->log_file = "sim_out";
    logger->sim_out = NULL;//file ptr
}

/*
  enter critical section of logging:
  checking for log file condition and writing
  to it, flushing the buffer afterwards
 */
void write_log(Logger *logger, char* line)
{
    pthread_mutex_lock(&logger->mutex_log);//critical writing
    
    if(logger->sim_out == NULL) {//don't open an open file
	logger->sim_out = fopen(logger->log_file, "a");
    }
    fprintf(logger->sim_out, "%s", line);
    fflush(logger->sim_out);//flush buffer
    
    pthread_mutex_unlock(&logger->mutex_log);
}

