// Name: Hou, Binbing
// Project: PA-1 (Simplebench)
// Instructor: Feng Chen
// Class: cs7700-sp18
// LogonID: cs770094

#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include<unistd.h>
#include <sys/time.h>  
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include "simplebench.h"

/*This file contains the functions used to collect the I/O traces*/

//clean the trace file; create it if not existing
int init_trace_file(char *output){

    FILE *ftrace = fopen(output, "w+");

    if (ftrace == NULL)
       return 1;

    fclose(ftrace);
   
    return 0;
}

// I/O tracing for each request
int io_tracing(char *output, float timestamp, float latency, int thread_id, char type, off_t offset, int size)
{
    static pthread_mutex_t tracelock;
    FILE *ftrace;

    float issue_stmp_sec = timestamp; 
    off_t offset_sectors = offset / 512; 
    int size_sectors = size / 512;
    float latency_ms = latency;

    pthread_mutex_lock(&tracelock);
    
    ftrace = fopen(output, "a+");   
    fprintf(ftrace,"%.6f %d %c %lld %d %.3f\n",issue_stmp_sec,thread_id,\
            type,(long long)offset_sectors,size_sectors,latency_ms); 
    fclose(ftrace); 

    pthread_mutex_unlock(&tracelock);

    return 0;
}
