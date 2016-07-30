#include "p.h"
  
    int sem_id=0;    
    int main(int argc, char *argv[])  
    {  
        char message = 'X';  
        int i = 0;  
      
        //创建信号量  
        sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);  
      
        if(argc > 1)  
        {  
            //程序第一次被调用，初始化信号量  
            if(!set_semvalue())  
            {  
                fprintf(stderr, "Failed to initialize semaphore\n");  
                exit(EXIT_FAILURE);  
            }  
            //设置要输出到屏幕中的信息，即其参数的第一个字符  
            message = argv[1][0];  
            sleep(2);  
        }  
        for(i = 0; i < 10; ++i)  
        {  
            //进入临界区  
            if(!semaphore_p())  
                exit(EXIT_FAILURE);  
            //向屏幕中输出数据  
            printf("%c", message);  
            //清理缓冲区，然后休眠随机时间  
            fflush(stdout);  
            sleep(rand() % 3);  
            //离开临界区前再一次向屏幕输出数据  
            printf("%c", message);  
            fflush(stdout);  
            //离开临界区，休眠随机时间后继续循环  
            if(!semaphore_v())  
                exit(EXIT_FAILURE);  
            sleep(rand() % 2);  
        }  
      
        sleep(10);  
        printf("\n%d - finished\n", getpid());  
      
        if(argc > 1)  
        {  
            //如果程序是第一次被调用，则在退出前删除信号量  
            sleep(3);  
            del_semvalue();  
        }  
        exit(EXIT_SUCCESS);  
    }  
      

