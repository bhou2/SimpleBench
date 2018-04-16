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
#include<unistd.h>
#include <sys/time.h>  
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include "simplebench.h"

off_t random_offset (off_t max_offset){
    
    // presented in sectors
    off_t max_sectors = max_offset / 512;
    off_t sector_num = 0;
    Timeval tmp;
    gettimeofday(&tmp, NULL);

    srand(1000000*tmp.tv_sec+tmp.tv_usec);
    sector_num = rand() % max_sectors;
    
    return sector_num * 512; // offset in Bytes
}

off_t seq_offset(off_t pre_offset, int request_size, off_t max_offset){

    off_t offset;

    offset = pre_offset + request_size;
    if (offset > max_offset)
        offset = 0;

    return offset;    
}
    
int open_file(char *file, char type, char direct){

    int fd;

    if (type == 'R'){
        if (direct == 'T')
            fd = open(file, O_RDONLY | O_DIRECT);
        else
            fd = open(file, O_RDONLY);
    }
    else{
        if (direct == 'T')
            fd = open(file, O_WRONLY |  O_DIRECT);
        else
            fd = open(file, O_WRONLY);
   }

   return fd;
}

int do_io(int fd, void *buffer, int request_size, char type){

    int n = 0;
    if (type == 'R')
        n = read(fd,buffer,request_size);
    else
        n = write(fd,buffer,request_size);
    
    return n;     
}

int get_thread_id(int thread_num){

   int i = 0;
   int thread_id = 0;
   pthread_t current_thread = pthread_self();
     
   for (i=0; i< thread_num; i++){
       if (pthread_equal(current_thread, threads[i])){
           thread_id = i; 
           break;
       }
   }

   return i;
}

void *alloc_buffer(char direct, int request_size){

   int pagesize;
   void *buffer;

   if (direct == 'F'){

     buffer = malloc(request_size+pagesize);     
     return buffer;
   }

   pagesize=getpagesize();
   posix_memalign (&buffer, pagesize, request_size);

   return buffer;
}

void *worker(void *args)
{
    int fd;
    off_t offset, max_offset;
    char *buffer;
    int request_size;
    int thread_id = 0;
    int io_result = 0;
    float latency = 0;
    float timestamp = 0;

    Timeval issue_time,complete_time,current_time;
 
    Parameters *paras = (Parameters *) args;

    fd = open_file(paras -> file, paras -> type, paras -> direct);

    if (fd == -1){
        printf("Open the file or device %s failed.\n",paras->file);
        printf("Make sure you have the permission to access the file.\n");
        close(fd);
        exit(1);
        return ((void *)0);
    }

    request_size = 1024 * paras -> size;
    max_offset = 1024 * 1024 * paras->range - request_size;

    thread_id = get_thread_id(paras -> thread_num);
    init_stat_per_thread(paras->stat,thread_id,paras->type);

    //buffer = alloc_buffer(paras->direct,request_size);
   
    //gettimeofday(&start_time, NULL);
    // current time, used to determine the stop time
    gettimeofday(&current_time, NULL);
    // the issue time of the request
    gettimeofday(&issue_time, NULL);

    // set the pointer of the first I/O
    offset = random_offset(max_offset);
    lseek(fd, offset, SEEK_SET); // set the position

    while (time_diff(current_time, start_time)/1000000.0 < paras -> duration){

        // alloc the buffer; for direct_io, the buffer has to be alligned
        buffer = alloc_buffer(paras->direct,request_size);

        // execute the I/O requests
        io_result = do_io(fd, buffer, request_size, paras -> type);
   
        // I/O completition time
        gettimeofday(&complete_time, NULL);

        if (io_result != request_size){
            printf("Thread %d %c %lld %d failed.\n",\
            	     thread_id,paras->type,(long long)offset/512,request_size/512);   
            printf("Check wheather the rest range is set excedding the size of the target file or device.\n");
            free(buffer);
	    exit(1);
        }
  
	// free the buffer
	free(buffer);

        // latency in milliseconds 
        latency = time_diff(complete_time,issue_time)/1000.0;
        // timestamp in seconds
        timestamp = time_diff(issue_time,start_time)/1000000.0;
   
        paras->stat[thread_id].io_num += 1;
        paras->stat[thread_id].latency_sum += latency;
       
        io_tracing(paras->output,timestamp,latency,thread_id,paras->type,offset,request_size);
 
        // the issue time of the next I/O
        gettimeofday(&issue_time, NULL);

       // determine the next offset and reset the pointer based on access pattern 
       if (paras -> pattern == 'S'){ //sequeitnal
            offset = seq_offset(offset,request_size,max_offset);    
            // wrap and reset the pointer; 
            // otherwise, the pointer will move automatically  
            if (offset == 0) 
               lseek(fd, 0, SEEK_SET);       
       }
       else{ // random, need to reset the position
            offset = random_offset(max_offset);
            lseek(fd, offset, SEEK_SET);
       }
     
       // check the current time
       gettimeofday(&current_time, NULL); //update the current time

    } // end of while

    close(fd);
    return ((void *)0); 
}
