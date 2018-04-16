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

/*
This file contains the functions used to collect test results
and calculate the values of different metrics.
*/

// init the Stat data structure of each thread
int init_stat_per_thread(Stat *stat, int thread_id, char type){

   int i = thread_id;
   stat[i].io_num = 0; 
   stat[i].latency_sum = 0;
   stat[i].type = type;
   
   return 0;
}

// init the Result data structure 
int init_result(Result *res){
  res -> duration = 0;
  res -> io_num = 0;
  res -> read_io_num = 0;
  res -> write_io_num = 0;
  res -> data_transfer = 0;
  res -> read_data_transfer = 0;
  res -> write_data_transfer = 0;
  res -> avg_latency = 0;
  res -> avg_read_latency = 0;
  res -> avg_write_latency = 0;
  res -> throughput = 0;
  res -> read_throughput = 0;
  res -> write_throughput = 0;
  res -> bandwidth = 0;
  res -> read_bandwidth = 0;
  res -> write_bandwidth = 0;
}

// calcluate the values of different metrics 
int result_stat(Stat *stat, Result *res, int thread_num, int size, char type, float duration){

   int i;
   long read_num = 0;
   long write_num = 0;
   float read_data_transfer = 0;
   float write_data_transfer = 0;
   float read_latency = 0;
   float write_latency = 0;
   float thread_io_num = 0;
   float thread_latency_sum = 0;

   for(i=0;i<thread_num;i++){
      thread_io_num = stat[i].io_num;
      thread_latency_sum = stat[i].latency_sum;
      if (type == 'R'){
          read_num += thread_io_num;
          read_data_transfer += (thread_io_num * size / 1024); // in MB
          read_latency += thread_latency_sum;
      }
      else{
          write_num += thread_io_num;
          write_data_transfer += (thread_io_num * size / 1024); // in MB
          write_latency += thread_latency_sum;
     }
   }
   
   res -> duration = duration;
   res -> read_io_num = read_num;
   res -> write_io_num = write_num;
   res -> io_num = read_num + write_num;
   res -> data_transfer = read_data_transfer + write_data_transfer;
   res -> read_data_transfer = read_data_transfer;
   res -> write_data_transfer = write_data_transfer;
   res -> avg_latency = (read_latency+write_latency) / res->io_num;
   if (read_num != 0)
       res -> avg_read_latency = read_latency / read_num;
   if (write_num != 0)
       res -> avg_write_latency = write_latency / write_num;
   res -> throughput = res->io_num / res->duration;
   res -> read_throughput = read_num / res->duration;
   res -> write_throughput = write_num / res-> duration;
   res -> bandwidth = res->data_transfer/res->duration;
   res -> read_bandwidth = res->read_data_transfer / res->duration;
   res -> write_bandwidth = res->write_data_transfer / res->duration;
 
   return 0;
}


// print the results on screen
void result_print(Result res){

  printf("\n");
  printf("Test Duration: %.1f second(s)\n", res.duration);
  printf("\n");
  printf("Number of requests \nAll: %ld \nReads: %ld \nWrites: %ld \n",\
         res.io_num,res.read_io_num,res.write_io_num);
  printf("\n");
  printf("Volume of data transfer \nAll: %ld MB \nReads: %ld MB \nWrites: %ld MB\n",\
         res.data_transfer,res.read_data_transfer,res.write_data_transfer);
  printf("\n");
  printf("Average latency \nAll: %.3f ms \nReads: %.3f ms \nWrites: %.3f ms\n",\
          res.avg_latency,res.avg_read_latency,res.avg_write_latency);
  printf("\n");
  printf("Throughput \nAll: %.3f IOPS \nReads: %.3f IOPS \nWrites: %.3f IOPS\n",\
          res.throughput,res.read_throughput,res.write_throughput);
  printf("\n");
  printf("Bandwidth \nAll: %.3f MB/s \nReads: %.3f MB/s \nWrites: %.3f MB/s \n",\
          res.bandwidth,res.read_bandwidth,res.write_bandwidth);
  printf("\n");

}

