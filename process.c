#include "process.h"
extern  int sem_id;  
int  flag = 1;
int set_semvalue()  
{  
    //用于初始化信号量，在使用信号量前必须这样做  
    union semun sem_union;  
  
    sem_union.val = 1;  
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)  
        return 0;  
    return 1;  
}  
  
 void del_semvalue()  
{  
    //删除信号量  
    union semun sem_union;  
  
    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)  
        fprintf(stderr, "Failed to delete semaphore :%s\n",strerror(errno),errno);
}  
  
 int semaphore_p()  
{  
    //对信号量做减1操作，即等待P（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_p failed :%s %d\n",strerror(errno),errno);  
        return 0;  
    }  
    return 1;  
}  
  
int semaphore_v()  
{  
    //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed :%s %d\n",strerror(errno),errno);  
        return 0;  
    }  
    return 1;  
}  


int sendMessageQueue_Boa(char * message,long msgType)
{

        struct msg_st data;  
        char buffer[BUFSIZ];  
        int msgid = -1;  
      
        //建立消息队列  
        msgid = msgget((key_t)1234, 4777 | IPC_CREAT);  
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return  -1;
        }  
            //输入数据  
        data.msg_type  =  msgType;                      
        strcpy(data.text, message);  
            //向队列发送数据  
        if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)  
           {  
                printf("msgsnd failed:%d\n",errno);  
               return -1;
           }
         else
           {
                printf("msgsnd successful :%s Type:%ld!\n",data.text,msgType);
           }  
        return 0;
	
}

int sendMessageQueue_Named(char * message,long msgType)
{
/*
a. msg_type == 0    返回消息队列中第一个消息，先进先出

b. msg_type > 0    返回消息队列中类型为tpye的第一个消息

c. msg_type < 0    返回消息队列中类型 <=  |type| 的数据；若这种消息有若干个，则取类型值最小的消息
*/
        struct msg_st data;  
        char buffer[BUFSIZ];  
        int msgid = -1;  
      
        //建立消息队列  
        msgid = msgget((key_t)444, 4777 | IPC_CREAT);  
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return  -1;
        }  
            //输入数据  
        data.msg_type = msgType;      //
        strcpy(data.text, message);  
            //向队列发送数据  
        if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)  
           {  
                printf("msgsnd failed:%d\n",errno);  
                return -1;
           }
         else
           {
                printf("msgsnd successful :msgText:%s msgType:%ld len:%d!\n",data.text,data.msg_type,strlen(data.text));
           }  
        return 0;
	
}
int sendMessageQueue_Function(char * message ,key_t key)
{

        struct msg_st data;  
        char buffer[BUFSIZ];  
        int msgid = -1;  
      
        //建立消息队列  
        msgid = msgget(key, 4777 | IPC_CREAT);  
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return  -1;
        }  
            //输入数据  
        data.msg_type = 1;                            //注意2  
        strcpy(data.text, message);  
            //向队列发送数据  
        if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)  
           {  
                printf("msgsnd failed:%d\n",errno);  
               return -1;
           }
         else
           {
                printf("msgsnd successful!\n");
           }  
        return 0;
}
char * recvMessageQueue_Block(char * waitStr ,long msgType)                                         //阻塞方法  (点名测试 重启)
{
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;    
        char * str=NULL;
        str = (char * ) malloc (sizeof(char)*10);
        msgid = msgget((key_t)444, 4777 | IPC_CREAT);                   //建立消息队列 (与otdrMain通信)   444
        if(msgid == -1)  
        {  
            printf("msgget failed with error: %d\n", errno);  
            return  "MSG Error"; 
        } 
        data.msg_type=msgType;                                          //只获取某一特定类型消息的第一个
        while(running)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgType,IPC_NOWAIT|MSG_NOERROR);
            if(strlen(data.text) !=0){  
               printf("Distory RecvMessage: %s msgType:%ld\n",data.text,data.msg_type);                     
               if(strncmp(data.text, waitStr, strlen(waitStr)) == 0){
                     printf("Useful RecvMessage: %s\n",data.text);  
                     strcpy(str,data.text);
                     running = 0;                                       //遇到1-OK结束 
                     //break;
                }
               strcpy(data.text,"");   
             }      
        } 

        pid_t backPID[MAX_PID_NUM];  
        int   retProcess;
                            
        retProcess = get_pid_by_name("/web/cgi-bin/BoaCom.cgi", backPID, MAX_PID_NUM);  
        
        if(retProcess == 1  ){
		if(msgctl(msgid, IPC_RMID, 0) == -1)                           //删除消息队列 
		{  
		    printf("msgctl(IPC_RMID) failed\n");  
		    return "MSG Error";    
		} 
        }
           
        return str;
}

void sigOutime(int signo){
    switch (signo){
        case SIGVTALRM:
            printf("Catch a signal -- SIGVTALRM \n");
            flag = 0;
            break;
    }
    return;
}
int  recvMessageQueue_Backstage(char *waitStr,long msgType)                                      //非阻塞方法+超时判断
{

        struct itimerval value, ovalue;  
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
                                           
        int rflags;
        int ret;        
        signal(SIGVTALRM, sigOutime);
        value.it_value.tv_sec = 1;
        value.it_value.tv_usec = 0;
        value.it_interval.tv_sec = 1;
        value.it_interval.tv_usec = 0;
        setitimer(ITIMER_VIRTUAL, &value, &ovalue);

        msgid = msgget((key_t)1234, 4777 | IPC_CREAT);                     //建立消息队列(与BOA服务器通信) 1234
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return 0;
             
        }  

        data.msg_type = msgType;  

        while(flag)                                                           //只获取某一特定类型消息 
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgType,IPC_NOWAIT|MSG_NOERROR);   //一定要加上MSG_NOERROR 这样服务器才能正常响应HTTP帧头!  否则出现异常情况:502错误，同时产生BoaCom.cgi僵尸进程

            if(strlen(data.text) !=0){  
               printf("RecvMessage: %s type:%ld\n",data.text,data.msg_type);                     
               if(strncmp(data.text, waitStr, strlen(waitStr)) == 0){
                     printf("Useful RecvMessage: %s\n",data.text);                      
                     ret = 1;                                       
                     break;
                }
               strcpy(data.text,"");
             } 
   
        }  

        if(msgctl(msgid, IPC_RMID, 0) == -1)                             //删除消息队列 
        {  
            printf("msgctl(IPC_RMID) failed\n");  
            return 0;    
        } 

        if(flag == 0) ret = 0;

        return ret; 


}
int recvMessageQueue_OTDR(char * backMSG, key_t key)                                         //阻塞方法  
{

        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
        long int msgtype = 0;                                            //只获取某一特定类型消息  
        int rflags;
        int ret;
         msgid = msgget(key, 4777 | IPC_CREAT);                   //建立消息队列 (与otdrMain通信)   444  
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return -1;
             
        }  

        rflags=IPC_NOWAIT|MSG_NOERROR;                                   //从队列中获取消息，直到遇到消息为止
          

        while(1)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, IPC_NOWAIT|MSG_NOERROR);  
           
            if(strncmp(data.text,backMSG, strlen(backMSG) ) == 0){   
                            
               if(key==CYCLE_MESSAGE_KEY)
                  printf("cycMain RecvMessage: %s = SendMessage:%s\n",data.text,backMSG);  
               if(key==ALARM_MESSAGE_KEY)   
                  printf("alarmMain RecvMessage: %s = SendMessage:%s\n",data.text,backMSG);    
               if(key==PROTECT_MESSAGE_KEY)
                  printf("ProtectMasterMain RecvMessage: %s = SendMessage:%s\n",data.text,backMSG);                  
                 
               ret=0;
               break;              
             }           
        }  
        

        if(msgctl(msgid, IPC_RMID, 0) == -1)                          //删除消息队列 
        {  
            printf("msgctl(IPC_RMID) failed\n");  
            return -1;    
        } 
        //strcpy(data.text,"");
        return ret; 
}

queue *Queue_Initiate(){
   queue * q=NULL;
   q=malloc(sizeof(queue));
   memset(q->value,0,MaxQueueSize);
   q->tail=0;
   q->head=0;
   q->count=0;
   return q;
}

int  Queue_Append(queue *q,int  value){
     if(q->count>0 && q->tail == q->head ){
        return 0;	
     }else{
     	q->value[q->tail] = value;
     	q->tail=(q->tail+1)%MaxQueueSize;
        q->count++;
     } 
     return 1;

}
int  Queue_Delete(queue *q,int *value){
    if(q->count==0){
        return 0;
    }else{ 
    	*value=q->value[q->head];
        q->head=(q->head+1)%MaxQueueSize;
        q->count--;
    }

    return 1;
}

int  Queue_isEmpty(queue *q){
     
     if(q->count == 0)return 1;
     else return 0;

}
int  Queue_getData(queue *q,int * value){
     if(q->count!=0){
         return 0;
     }else{
         *value =q->value[q->head];
         return 1;
     }
}


char *basename(const char *path)
{
        register const char *s;
        register const char *p;

        p = s = path;

        while (*s) {
            if (*s++ == '/') {
                p = s;
            }
        }

        return (char *) p;
    }

     /* 根据进程名称获取PID, 比较 base name of pid_name
      * pid_list: 获取PID列表
      * list_size: 获取PID列表长度
      * RETURN值说明:
      *              < 0: 
      *              >=0: 发现多少PID, pid_list 将保存已发现的PID
      */
int get_pid_by_name(const char* process_name, pid_t pid_list[], int list_size)
    {
    #define  MAX_BUF_SIZE       256

        DIR *dir;
        struct dirent *next;
        int count=0;
        pid_t pid;
        FILE *fp;
        char *base_pname = NULL;
        char *base_fname = NULL;
        char cmdline[MAX_BUF_SIZE];
        char path[MAX_BUF_SIZE];

        if(process_name == NULL || pid_list == NULL)
            return -EINVAL;

        base_pname = basename(process_name);
        if(strlen(base_pname) <= 0)
            return -EINVAL;

        dir = opendir("/proc");
        if (!dir)
        {
            return -EIO;
        }
        while ((next = readdir(dir)) != NULL) {
            /* skip non-number */
            if (!isdigit(*next->d_name))
                continue;

            pid = strtol(next->d_name, NULL, 0);
            sprintf(path, "/proc/%u/cmdline", pid);
            fp = fopen(path, "r");
            if(fp == NULL)
                continue;

            memset(cmdline, 0, sizeof(cmdline));
            if(fread(cmdline, MAX_BUF_SIZE - 1, 1, fp) < 0){
                fclose(fp);
                continue;
            }
            fclose(fp);
            base_fname = basename(cmdline);

            if (strcmp(base_fname, base_pname) == 0 )
            {
                if(count >= list_size){
                    break;
                }else{
                    pid_list[count] = pid;
                    count++;
                }
            }
        }
        closedir(dir) ;
        return count;
    }

    /* 如果进程已经存在, return true */
    int is_process_exist(const char* process_name)
    {
        pid_t pid;

        return (get_pid_by_name(process_name, &pid, 1) > 0);
    }
