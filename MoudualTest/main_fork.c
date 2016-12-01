/*exec函数示例*/  
#include <stdio.h>  
#include <unistd.h>  
  
int main(void)  
{  
    int flag;  
    pid_t pid;  
    char *const argv[] = {"%U", "--user-data-dir=/home/Administrator/.chromiun", NULL};  
    //exec把当前进程印象替换成新的程序文件，故调用进程被覆盖  
  
    // 如果不指定全路径，则只检查PATH变量中存储的命令  
    if((pid = fork())==0) {  
        printf("in child process 1......\n");  
        //flag = execvp("./hello", NULL);  
        //envp变量的用  
        sleep(1);
         printf("sleep  1 over!\n");  
        char *envp[]={"PATH=.", NULL};  
        flag = execve("hello", NULL, envp);  
        if(flag == -1)  
            printf("exec error!\n");  
    }  
  
    if((pid = fork())==0) {  
        printf("in child process 2......\n");  
        //执行ls命令  
        sleep(2);
         printf("sleep  2 over!\n"); 
        flag = execlp("ls", "-al", NULL);  
        if(flag == -1)  
            printf("exec error!\n");  
    }  
      
    if((pid = fork())==0) {  
        printf("in child process 3......\n");  
        //启动chrome浏览器  
        sleep(3);
         printf("sleep  3 over!\n"); 
        flag = execv("/usr/bin/chromium-browser", argv);  
        if(flag == -1)  
            printf("exec error!\n");  
    }  
    printf("in parent process ......\n");  
    return 0;  
}  
