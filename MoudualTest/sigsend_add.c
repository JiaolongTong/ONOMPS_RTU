#include <signal.h>

#include <sys/types.h>

#include <unistd.h>

#include <stdlib.h>  
#include <stdio.h>  
#include <string.h> 

#include "process.h"

void send(void);
void main()
{

  send();
  printf("send signal success!\n");
}


void send(void)
{
       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/cycMain";
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;
       mysigval.sival_int =10;
       for(n=0;n<ret;n++){  
        printf("cycPID:%u\n", cycPID[n]);
       // if(kill(cycPID[n],signum)==-1)   printf("send error\n");

        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  
}
