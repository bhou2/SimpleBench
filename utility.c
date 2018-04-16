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
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "simplebench.h"

/*This file contains the utility functions*/

// check whether the user has the permission 
// to test the target file or device
int permission_check (char *file, char type)
{
   int ret;

   /* Check file existence. */
   ret = access (file, F_OK);

   if (ret != 0){
      printf ("%s does not exist.\n", file);
      return ret;
   }

   /*check file read access*/
   ret = access (file, R_OK);

   if (ret != 0 && type == 'R'){
       printf ("%s is not readable.\n", file);
       return ret;
   }

   /* Check write access. */
   ret = access (file, W_OK);

   if (ret != 0 && type == 'W'){
      printf ("%s is not writable.\n", file);
      return ret;
   }

   return 0;
}

// calculate the difference between two timestamps
long time_diff(Timeval end, Timeval start){

    long seconds; 
    long micro_seconds; 
    long diff_micro_seconds;

    seconds = end.tv_sec - start.tv_sec;
    micro_seconds = end.tv_usec - start.tv_usec;
    diff_micro_seconds = seconds * 1000000 + micro_seconds;

    return diff_micro_seconds;
}
