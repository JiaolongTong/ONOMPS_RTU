#include "process.h"
extern  int sem_id;  
int  flag = 0;
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
        fprintf(stderr, "Failed to delete semaphore\n");  
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
        fprintf(stderr, "semaphore_p failed\n");  
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
        fprintf(stderr, "semaphore_v failed\n");  
        return 0;  
    }  
    return 1;  
}  


int sendMessageQueue(char * message)
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

int sendMessageQueue_B(char * message)
{

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

char * recvMessageQueue_A(void)                                         //阻塞方法  (点名测试)
{
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
        long int msgtype = 0;                                           //只获取某一特定类型消息 
         
        char * str;
         str = (char * ) malloc (sizeof(char)*10);
        msgid = msgget((key_t)444, 4777 | IPC_CREAT);                   //建立消息队列 (与otdrMain通信)   444
        if(msgid == -1)  
        {  
            printf("msgget failed with error: %d\n", errno);  
            return  "MSG Error"; 
        } 

        while(running)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgtype,IPC_NOWAIT|MSG_NOERROR);
            if(strlen(data.text) !=0){                     
               if(strncmp(data.text, "1-OK", 4) == 0){
                     printf("RecvMessage: %s\n",data.text);  
                     strcpy(str,data.text);
                     running = 0;                                       //遇到1-OK结束 
                     break;
                }
             }      
        } 
        strcpy(data.text,"");       
        return str;
}



char * recvMessageQueue_B(void)                                         //非阻塞方法
{
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
        long int msgtype = 0;                                            //只获取某一特定类型消息  
        int rflags;
        char * str;
         msgid = msgget((key_t)444, 4777 | IPC_CREAT);                   //建立消息队列 (与otdrMain通信)   444  
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return "MSG Error";
             
        }  

        rflags=IPC_NOWAIT|MSG_NOERROR;                                   //从队列中获取消息，直到遇到消息为止
          

        while(1)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, IPC_NOWAIT|MSG_NOERROR);   //一定要加上MSG_NOERROR 这样服务器才能正常响应HTTP帧头!  否则出现异常情况:502错误，同时产生BoaCom.cgi僵尸进程
            if(strlen(data.text) !=0){                                   //因为在非阻塞情况下，消息队列一旦发现有错误消息就会立即产生信号（在不设置MSG_NOERR情况下），将自己杀死，进而无法向中心服务器回复HTTP帧头。
               printf("RecvMessage: %s\n",data.text);                       
               //running = 0;  
               break;              
             }           
        }  
        

        if(msgctl(msgid, IPC_RMID, 0) == -1)                          //删除消息队列 
        {  
            printf("msgctl(IPC_RMID) failed\n");  
            return "MSG Error";    
        } 

        str = (char * ) malloc (sizeof(char)*10);
        strcpy(str,data.text);
        return str; 
}


void sigOutime(int signo){
    switch (signo){
        case SIGVTALRM:
            printf("Catch a signal -- SIGVTALRM \n");
            //signal(SIGVTALRM, sigOutime);
            flag = 1;
            break;
    }
    return;
}
char * recvMessageQueue_C(void)                                      //非阻塞方法+超时判断
{

        struct itimerval value, ovalue;  
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
        long int msgtype = 0;                                        //只获取某一特定类型消息  
        int rflags;
        char * str;        
        signal(SIGVTALRM, sigOutime);
        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = 100000;
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_usec = 400000;
        setitimer(ITIMER_VIRTUAL, &value, &ovalue);
        str = (char * ) malloc (sizeof(char)*10);
        msgid = msgget((key_t)1234, 4777 | IPC_CREAT);              //建立消息队列(与BOA服务器通信) 1234
        if(msgid == -1)  
        {  
            printf( "msgget failed with error: %d\n", errno);  
            return "MSG Error";
             
        }  

        rflags=IPC_NOWAIT|MSG_NOERROR;                              //从队列中获取消息，直到遇到消息为止
          

        while(1)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, IPC_NOWAIT|MSG_NOERROR);   //一定要加上MSG_NOERROR 这样服务器才能正常响应HTTP帧头!  否则出现异常情况:502错误，同时产生BoaCom.cgi僵尸进程
            if(strlen(data.text) !=0){                              //因为在非阻塞情况下，消息队列一旦发现有错误消息就会立即产生信号（在不设置MSG_NOERR情况下），将自己杀死，进而无法向中心服务器回复HTTP帧头。
               printf("RecvMessage: %s\n",data.text);                       
               strcpy(str,data.text);
               break;              
             } 
            if(flag){
               printf("Wait cycMain message time out");
               strcpy(str,"Timeout");
               flag=0;
               break;
	    }          
        }  
        

        if(msgctl(msgid, IPC_RMID, 0) == -1)                        //删除消息队列 
        {  
            printf("msgctl(IPC_RMID) failed\n");  
            return "MSG Error";    
        } 



        return str; 


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
