#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED
#define MAX_PID_NUM     32  
#define MAX_TEXT 512   //消息队列最大内容
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>  
#include <fcntl.h>  
#include <string.h>  
#include <fcntl.h>  
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>  

#include <signal.h>
 

union semun   
{  
    int val;  
    struct semid_ds *buf;  
    unsigned short *arry;  
};                                             //信号量，做进程间同步
  
struct msg_st                                  //消息队列，服务器判断设置是否成功
{  
        long int msg_type;  
        char text[BUFSIZ];  
};  
  
 int set_semvalue();  
 void del_semvalue();  
 int semaphore_p();  
 int semaphore_v();  

int sendMessageQueue(char * message);
int sendMessageQueue_B(char * message);
char * recvMessageQueue_A(void);
char * recvMessageQueue_B(void);
char * recvMessageQueue_C(void);
void   sigOutime(int signo);


char *basename(const char *path);
int get_pid_by_name(const char* process_name, pid_t pid_list[], int list_size);
int is_process_exist(const char* process_name);
#endif // PROCESS_H_INCLUDE
