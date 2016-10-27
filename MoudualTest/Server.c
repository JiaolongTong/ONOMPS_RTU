#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/msg.h>  
#include <sys/ipc.h>  
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
}msg_rbuf;  
struct msgbuf2{  
    int type;  
    int a;  
    int b;  
    char text[N];  
}msg_sbuf;  
  
int main(){  
    key_t key;  
    key = ftok(Pathname,ID);  
    int msgid;  
    msgid = msgget(key,0);//这里和上面的有不同  
    if(msgid == -1){  
        perror("Msgqueue has exist");  
        exit(1);  
    }  
    while(1){  
        pid_t pid;  
        pid = fork();  
        if(pid > 0){//parent  
            wait(NULL);  
            msg_sbuf.type = SENDMSG;  
            char str[N];  
            printf("Please input info: name , a, b\n");  
            scanf("%s %d %d",str,&msg_sbuf.a,&msg_sbuf.b);  
            strcpy(msg_sbuf.text,str);  
            printf("Parent process send msg, pid = %d\n",getpid());  
            msgsnd(msgid, &msg_sbuf,sizeof(msg_sbuf),IPC_NOWAIT);  
        }else if(pid == 0){  
            msgrcv(msgid,&msg_rbuf,sizeof(msg_rbuf),RECVMSG,0);  
            printf("Child process recv msg, pid = %d\n",getpid());  
            printf("xkey: Receive: %s, sum %d + %d = %d \n",msg_rbuf.text,msg_rbuf.a,msg_rbuf.b,msg_rbuf.a+msg_rbuf.b);  
            exit(1);  
        }  
    }  
    msgctl(msgid,IPC_RMID,NULL);  
    exit(0);  
}  
