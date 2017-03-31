#include <signal.h>

#include <sys/types.h>

#include <unistd.h>

#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#define MaxQueueSize 1024
static int PV=0;
queue * myQueue=NULL;
static int num=0;
typedef struct queue{   //FIFO
   int value[MaxQueueSize];
   int head;
   int tail;
   int count;
}queue;

int in_queue(queue *q,int value){

     if(q->count>0 && q->tail == q->head ){
        return 0;	
     }else{
     	q->value[q->tail] = value;
     	q->tail=(q->tail+1)%MaxQueueSize;
        q->count++;
     } 
     return 1;
}

int out_queue(queue *q,int *value){
    if(q->count==0){
        return 0;
    }else{ 
    	*value=q->value[q->head];
        q->head=(q->head+1)%MaxQueueSize;
        q->count--;
    }

    return 1;
}
void init_queue(queue *q){

   q=malloc(sizeof(queue));
   memset(q->value,0,MaxQueueSize);
   q->tail=0;
   q->head=0;
   q->count=0;
}


int queue_headle(){
    int data,ret;
    if(myQueue->count==0){
            //printf("the link is null\n");
            return 0;
    }else{
	    ret=out_queue(myQueue,&data);
	    printf("the int value is %d  queue_len:%d head:%d tail:%d \n",data,myQueue->count,myQueue->head,myQueue->tail );
    }
    return ret;
}
void new_op(int,siginfo_t*,void*);

int main(int argc,char**argv)

{

       struct sigaction act;

       int sig;

       pid_t pid;       

 

       pid=getpid();

       sig=SIGRTMIN;  

 

       sigemptyset(&act.sa_mask);

       act.sa_sigaction=new_op;

       act.sa_flags=SA_SIGINFO;

       if(sigaction(sig,&act,NULL)<0)

       {

              printf("install sigal error\n");

       }

       init_queue(myQueue);
       while(1)

       {
              queue_headle(); 
              usleep(50000);

       }

}


void new_op(int signum,siginfo_t *info,void *myact)

{
      while(PV==1);
      PV=1;
      in_queue(myQueue,info->si_int);
      PV=0;

}
