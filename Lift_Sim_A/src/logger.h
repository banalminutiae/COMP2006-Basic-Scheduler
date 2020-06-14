//FILE: LOGGER
//AUTHOR: ANDREW LI - 19460677
//DESCRIPTION: Logfile implementation with inbuilt mutual exclusion


#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <stdio.h>

typedef struct Logger{

    FILE *sim_out;
    pthread_mutex_t mutex_log;
    char *log_file;
    
}Logger;

void init_log(Logger*);
void write_log(Logger*, char*);

#endif
