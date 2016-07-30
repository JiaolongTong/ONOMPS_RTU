    #include <unistd.h>  
    #include <sys/types.h>  
    #include <sys/stat.h>  
    #include <fcntl.h>  
    #include <stdlib.h>  
    #include <stdio.h>  
    #include <string.h>  
    #include <sys/sem.h>  
      
    union semun  
    {  
        int val;  
        struct semid_ds *buf;  
        unsigned short *arry;  
    };  
        
     int set_semvalue();  
     void del_semvalue();  
     int semaphore_p();  
     int semaphore_v();  
