#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED
#define MAX_PID_NUM     32  
#define MaxQueueSize 1024
typedef struct queue{   //FIFO
   int value[MaxQueueSize];
   int head;
   int tail;
   int count;
}queue;

int  Queue_Append(queue *q,int  value);
int  Queue_Delete(queue *q,int *value);
void Queue_Initiate();
int  Queue_isEmpty(queue *q);
int  Queue_getData();
char *basename(const char *path);
int get_pid_by_name(const char* process_name, pid_t pid_list[], int list_size);
int is_process_exist(const char* process_name);
#endif // PROCESS_H_INCLUDE
