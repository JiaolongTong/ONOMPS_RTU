/******************************************
*功能:Linux应用层系统时间写入RTC时钟的方法
*时间:2016-4-15
*作者:Jack Cui
*******************************************/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <bits/types.h>
#include <linux/rtc.h>



/*************************************************
*函数名        :   System_SetTime
*功能         :   写入系统时间
*使用方法   :   char* dt = "2016-04-15 21:00:00";
                System_SetTime(dt);
**************************************************/
int System_SetTime(char* dt)
{
    struct rtc_time tm;
    struct tm _tm;
    struct timeval tv;
    time_t timep;

    sscanf(dt,"%d-%d-%d %d:%d:%d",&tm.tm_year,&tm.tm_mon,&tm.tm_mday,&tm.tm_hour,&tm.tm_min,&tm.tm_sec);
    _tm.tm_sec = tm.tm_sec;
    _tm.tm_min = tm.tm_min;
    _tm.tm_hour = tm.tm_hour;
    _tm.tm_mday = tm.tm_mday;
    _tm.tm_mon = tm.tm_mon - 1;
    _tm.tm_year = tm.tm_year - 1900;

    timep = mktime(&_tm);                               
    tv.tv_sec=timep; 
    tv.tv_usec=0;
    
    if(settimeofday(&tv, (struct timezone *) 0) < 0)
    {
        printf("Set system datetime error!\n");
        return -1;
    }   
    return 0;
}

int main(void)
{
    char *dt = "2016-4-15 21:00:00";
    pid_t fpid;                 //fpid表示fork函数返回的值
    fpid=fork(); 
    if (fpid < 0)               //创建子进程失败
        printf("error\n"); 
    else if (fpid == 0) 
    {
        char * args[] = {"/home/nfsroot/hwclock.sh", NULL};
        if(-1 == (execve("/home/nfsroot/hwclock.sh",args,NULL)))
        {
            perror("execve");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        System_SetTime(dt);
    }
    return 0;

}
