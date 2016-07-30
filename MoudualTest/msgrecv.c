    #include <unistd.h>  
    #include <stdlib.h>  
    #include <stdio.h>  
    #include <string.h>  
    #include <errno.h>  
    #include <sys/msg.h>  

#include <signal.h>
#include <time.h>
#include <sys/time.h> 

     
    struct msg_st  
    {  
        long int msg_type;  
        char text[BUFSIZ];  
    };  
      




    int main()  
    {  
        int msgid = -1;  
        int running = 1;  
        struct msg_st data;  
        long int msgtype = 0; //只获取某一特定类型消息  
        int rflags;
        struct itimerval value, ovalue, value2;          //(1)
        //建立消息队列  
        msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
        if(msgid == -1)  
        {  
            fprintf(stderr, "msgget failed with error: %d\n", errno);  
            exit(EXIT_FAILURE);  
        }  

        rflags=IPC_NOWAIT|MSG_NOERROR;
        //从队列中获取消息，直到遇到end消息为止  

        while(running)  
        {  
            msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, IPC_NOWAIT);
            if(strlen(data.text) !=0){
               printf("RecvMessage: %s\n",data.text);                       
               if(strncmp(data.text, "end", 3) == 0) 
                     running = 0;                //遇到end结束 
             }
            strcpy(data.text,"");
               
        }  
        //删除消息队列  
        if(msgctl(msgid, IPC_RMID, 0) == -1)  
        {  
            fprintf(stderr, "msgctl(IPC_RMID) failed\n");  
            exit(EXIT_FAILURE);  
        }  
        exit(EXIT_SUCCESS);  
    }  
