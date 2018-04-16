// Name: Hou, Binbing
// Project: PA-1 (Simplebench)
// Instructor: Feng Chen
// Class: cs7700-sp18
// LogonID: cs770094

#include <sys/time.h>  
#include <time.h>

// this is a system data structure used 
// to otbain current time in microseconds
typedef struct timeval Timeval;

// this data structure is used to collect the test results of each thread
typedef struct{
  float latency_sum; //
  long  io_num; // the number of requests
  char type; // read or write
}Stat;

// this data is used to organize the parameters and
// shared by all the threads 
typedef struct{
    int duration;
    char file[250];
    int range;
    int size;
    char type;
    char pattern;
    char direct;
    char output[250];
    int thread_num;
   // each thread has a private Stat structure 
   // to avoid the ovherhad of thread synchronization
    Stat *stat; 
}Parameters;

// this data structure is used to present the test results
typedef struct{
  float duration;
  long io_num;
  long read_io_num;
  long write_io_num;
  long data_transfer;
  long read_data_transfer;
  long write_data_transfer;
  float avg_latency;
  float avg_read_latency;
  float avg_write_latency;
  float throughput;
  float read_throughput;
  float write_throughput;
  float bandwidth;
  float read_bandwidth;
  float write_bandwidth;
}Result;

// the number of parameters
// currently, it is 9
#define PARA_NUM 9

// for parameters checking
#define DUR 0
#define FIL 1
#define RNG 2
#define SIZ 3
#define TYP 4
#define PAT 5
#define QDE 6
#define DRT 7
#define OUT 8

/* it is not elegant to set global variables
however, the following variables are shared by all the threads
it is logically clearer to set them globally visible */

// the thread array, particulary used to determine 
// the id of the current running thread
pthread_t *threads;
// the start and end timpstamps of the threads 
Timeval start_time, end_time;

/****************************parameter.c***************************************/
// parse the parameters set by users
int parmeters_parsing_and_checking(char **argv, int argc, Parameters *paras);
// check each parameter
int type_check(char *type_str);
int pattern_check(char *pattern_str);
int direct_check(char *direct_str);
int digital_check(char s);
int value_string_check(char *str);
int value_check(int value);
int parameters_errors(int *checker, int para_num);
int file_exist_check(char *file);

/*****************************utility.c****************************************/
// calcualte the time interval
long time_diff(Timeval end, Timeval start);  
// check whether the user has specific permissions to test the file or device
int permission_check(char *file, char type);

/*******************************worker.c****************************************/
// generate a random offset
off_t random_offset (off_t max_offset);
// check whether a sequential I/O reaches the boundary
off_t seq_offset(off_t pre_offset, int request_size, off_t max_offset);
// open the target file or device with the specified flag and mode
int open_file(char *file, char type, char direct);
// execute I/O requests
int do_io(int fd, void *buffer, int request_size, char type);
// get the id of the current running thread
int get_thread_id(int thread_num);
// allocate the buffer for each thread; 
// for direct I/O, the buffer has to be alligned with the page size
void *alloc_buffer(char direct, int request_size);
// the worker for reads or writes
void *worker(void *args);

/*********************************trace.c*****************************************/
// init and create the trace file
int init_trace_file(char *output);
// trace each I/O
int io_tracing(char *output, float timestamp, float latency, int thread_id,\
               char type, off_t offset, int size);

/***********************************stat.c*****************************************/
// init the Stat data structure
int init_stat_per_thread(Stat *stat, int thread_id, char type);
// init the Result data structure
int init_result(Result *res);
// calculate the metrics
int result_stat(Stat *stat, Result *res, int thread_num, int size, char type, float duration);
// print the results
void result_print(Result res);
