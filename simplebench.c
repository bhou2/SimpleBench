// Name: Hou, Binbing
// Project: PA-1 (Simplebench)
// Instructor: Feng Chen
// Class: cs7700-sp18
// LogonID: cs770094

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>  
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include "simplebench.h"

/* The main function controls the flow of testing */

int main(int argc, char ** argv)
{

    int i, err;
    float real_duration; 

    Parameters paras;
    Result res;

    // parse the parameters
    if (parmeters_parsing_and_checking(argv, argc, &paras) != 0){
        printf("Please refer to the README file for parameter settings.\n");
        return 0;
    }

    // check the access permission for the file or device to be tested
    if (permission_check(paras.file, paras.type) != 0){
        printf("Make sure that you have the permission to test the file or device\n");
        return 0;
    }

    // check the permission to create and open the trace file
    if (init_trace_file(paras.output) != 0){
        printf("Failed to create or open the trace file %s \n", paras.output);
        printf("Make sure that you have the permission \n");
        return 0;
    }

    // init related data structures 
    threads = (pthread_t*)malloc(paras.thread_num * sizeof(pthread_t));
    // allocate the State data structure per thread
    paras.stat = (Stat *)malloc(paras.thread_num * sizeof(Stat));
    // init the Result data structure
    init_result(&res);

    // record the start time
    gettimeofday(&(start_time), NULL);

    // create and start the threads    
    for (i = 0; i < paras.thread_num; i++){
        err = pthread_create(&(threads[i]), NULL, &worker, (void*)&paras);
        if (err != 0){
            printf("Thread creation failed :[%s]", strerror(err));
            return 0;
        }
    }

    // wait until all the threads are completed
    for (i = 0; i< paras.thread_num; i++) 
         pthread_join(threads[i], NULL);

    // record the end time
    gettimeofday(&(end_time), NULL);
   
    // calculate thr real duration   
    real_duration = time_diff(end_time,start_time) /1000000.0;
    // calcluate the metrics
    result_stat(paras.stat, &res, paras.thread_num, paras.size, paras.type, real_duration);
    // print the results
    result_print(res);
 
    return 0;
}
