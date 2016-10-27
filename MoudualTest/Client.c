#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/msg.h>  
  
#define N 1024  
#define Pathname "/tmp/xkeyideal"  
#define MODE IPC_CREAT|IPC_EXCL|0666  
#define ID 27  
#define SENDMSG 1  
#define RECVMSG 2  
  
struct msgbuf{  
    int type;  
    int a;  
    int b;  
    char text[N];  
}msg_rbuf,msg_sbuf;  
  
int main(){  
    key_t key;  
    key = ftok(Pathname,ID);  
    int msgid;  
    msgid = msgget(key,MODE);  
    if(msgid == -1){  
        printf("error");  
        exit(1);  
    }  
    while(1){  
        pid_t pid,pid_wait;  
        pid = fork();  
        if(pid > 0){  
            pid_wait = waitpid(pid,NULL,0);//父进程等待子进程先执行  
            printf("Parent process recv msg,pid = %d\n",getpid());  
            msgrcv(msgid,&msg_rbuf,sizeof(msg_rbuf),SENDMSG,0);  
            int a = msg_rbuf.a;  
            int b = msg_rbuf.b;  
            printf("color: Receive: %s, sum %d + %d = %d\n",msg_rbuf.text,a,b,a+b);  
        }else if(pid == 0){  
            char str[N];  
            printf("Child process send msg, pid = %d\n",getpid());  
            printf("Please input msg info str ,a ,b\n");  
            scanf("%s %d %d",str,&msg_sbuf.a,&msg_sbuf.b);  
            strcpy(msg_sbuf.text,str);  
            msg_sbuf.type = RECVMSG;  
            msgsnd(msgid,&msg_sbuf,sizeof(msg_sbuf),IPC_NOWAIT);  
            exit(0);  
        }  
    }  
    msgctl(msgid,IPC_RMID,NULL);  
    exit(0);  
}  




