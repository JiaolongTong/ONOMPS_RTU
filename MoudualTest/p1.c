#include "p.h"

extern  int sem_id;  
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
