#include <stdio.h>
#include <unistd.h>
int main ()
{
    int flag;  
    pid_t pid;  
    if((pid = fork())==0) {  
        printf("Start Reboot System......\n");    
        flag = execl("/etc/boa/reboot.sh","reboot.sh","5"); 
        if(flag == -1)  
            printf("exec error!\n");  
        
    }  
    return 0;
}
