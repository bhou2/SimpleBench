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
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "simplebench.h"

/* This file contains the functions to parse and check the parameters*/

// check the type
int type_check(char *type_str){

   int ret = 0;
 
   char type = type_str[0];

   if (type != 'R' && type != 'W')
       ret =1;

   if (type_str[1] != '\0')
       ret = 1;

   return ret;
}

// check the pattern
int pattern_check(char *pattern_str){

   int ret = 0;
  
   char pattern = pattern_str[0];

   if (pattern != 'R' && pattern != 'S')
       ret = 1;

   if (pattern_str[1] != '\0')
       ret = 1;
 
   return ret;
}

// check the direct_io flag
int direct_check(char *direct_str){

   int ret = 0;

   char direct = direct_str[0];

   if (direct != 'T' && direct != 'F')
       ret =1;

   if (direct_str[1] != '\0')
       ret = 1;

   return ret;
}

// check whether a character is a digit
int digital_check(char s){

  int ret = 0;

  if (s >= '0' && s <= '9')
      ret = 0;
  else
      ret = 1;

  return ret;
}

// check whether a string is a number
int value_string_check(char *str){

   int i = 0, ret = 0;

   if (str == NULL){
       ret = 1;
       return ret;
   }

   if (digital_check(str[0]) != 0){ // check the fist
       ret = 1;
       return ret;
   }

   for (i = 1; str[i] != '\0'; i++){ // check the middle
      if ((digital_check(str[i]) != 0) && (str[i] != '.')){
          ret = 1;
          break;
      }
   }

  if ((i > 1) && (str[i-1] == '.')) // check the last
      ret = 1;

  return ret;
}

// check the value
int value_check(int value){
    
    int ret = 0;

    if (value < 1)
        ret = 1;
 
    return ret;
}

// check whether the target file or device exists
int file_exist_check(char *file){   

   int ret = 0;
 
   ret = access (file, F_OK);
   if (ret != 0)
       ret = 1;

   return ret; 
}

// parse and check the parameters
int parmeters_parsing_and_checking(char **argv, int argc, Parameters *paras)
{
   int i;
   int checker[PARA_NUM]; 
    
   // init the checker array
   for (i = 0; i < PARA_NUM; i++)
       checker[i] = -1;

   // parsing and checking
   for (i=1; i < argc-1; i++){
       
       if (strcmp(argv[i], "-e") == 0){
             checker[DUR] = value_string_check(argv[i+1]);             
             sscanf(argv[i+1], "%d", &(paras -> duration));
             checker[DUR] += value_check(paras -> duration);
             continue;
       }
       else if (strcmp(argv[i], "-f") == 0){
           strcpy(paras -> file, argv[i+1]);
           checker[FIL] = file_exist_check(paras -> file);
           continue;
       }
       else if (strcmp(argv[i], "-r") == 0){
            checker[RNG] = value_string_check(argv[i+1]);
            sscanf(argv[i+1], "%d", &(paras -> range));
            checker[RNG] += value_check(paras -> range);
           continue;
       }
       else if (strcmp(argv[i], "-s") == 0){
            checker[SIZ] = value_string_check(argv[i+1]);
            sscanf(argv[i+1], "%d", &(paras -> size));
            checker[SIZ] += value_check(paras -> size);
            continue;
       }
       else if (strcmp(argv[i], "-t") == 0){
             checker[TYP] = type_check(argv[i+1]);
             paras -> type = argv[i+1][0];
             continue;
       }
       else if (strcmp(argv[i], "-p") == 0){
           checker[PAT] = pattern_check(argv[i+1]);
           paras -> pattern = argv[i+1][0];      
           continue;
       }
       else if (strcmp(argv[i], "-o") == 0){
           strcpy(paras -> output, argv[i+1]);
           checker[OUT] = 0;
           continue;
       }
       else if (strcmp(argv[i], "-q") == 0){
           checker[QDE] = value_string_check(argv[i+1]);
           sscanf(argv[i+1], "%d", &(paras -> thread_num));
           checker[QDE] += value_check(paras -> thread_num);
           continue;
       }
       else if (strcmp(argv[i], "-d") == 0){
           checker[DRT] = direct_check(argv[i+1]);
           paras -> direct = argv[i+1][0]; 
           continue;
       }   
    }
   
   // report the parameter errors
   return parameters_errors(checker, PARA_NUM);
}

// report the errors of the parameters if existing
int parameters_errors(int *checker, int para_num){

   int ret = 0;
   int i;

   for (i = 0; i < para_num; i++){
      // correctly set
      if (checker[i] == 0)
          continue;
      // not set
      else if (checker[i] == -1){
          ret = 1;
          switch(i){
          case DUR: printf("The parameter -e for test duration is not set.\n"); break;
          case FIL: printf("The parameter -f for the file or device to be tested is not set.\n"); break;
          case RNG: printf("The parameter -r for test range is not set.\n"); break;
          case SIZ: printf("The parameter -s for request size is not set.\n"); break;
          case TYP: printf("The parameter -t for request type is not set.\n"); break;
          case PAT: printf("The parameter -p for request pattern is not set.\n"); break;
          case QDE: printf("The parameter -q for the thread number is not set.\n"); break;
          case DRT: printf("The parameter -d for direct_io is not set.\n"); break;
          case OUT: printf("The parameter -o for the trace file is not set.\n"); break;
          }
      }
      // wrong set
      else{
          ret = 1;
          switch(i){
          case DUR: printf("The parameter -e for test duration is not correctly set.\n"); break;
          case FIL: printf("The parameter -f is set with a file or device that does not exist.\n"); break;
          case RNG: printf("The parameter -r for test range is not correctly set.\n"); break;
          case SIZ: printf("The parameter -s for request size is not correctly set.\n"); break;
          case TYP: printf("The parameter -t for request type is not correctly set.\n"); break;
          case PAT: printf("The parameter -p for request pattern is not correctly set.\n"); break;
          case QDE: printf("The parameter -q for thread number is not correctly set.\n"); break;
          case DRT: printf("The parameter -d for direct_io is not correctly set.\n"); break;
          case OUT: printf("The parameter -o for the trace file is not correctly set.\n"); break;
          }
     }
  }

  return ret;

}
