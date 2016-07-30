#include <sys/types.h>

#include <sys/stat.h>

#include <errno.h>

#include <fcntl.h>

#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  

#define FIFO_SERVER "./fifoserver"

 

void main(int argc,char** argv)

{

       char r_buf[4096*2];

       int  fd;

       int  r_size;

       int  ret_size;

       r_size=atoi(argv[1]);

       printf("requred real read bytes %d\n",r_size);

       memset(r_buf,0,sizeof(r_buf));

       fd=open(FIFO_SERVER,O_RDONLY|O_NONBLOCK,0);

       //fd=open(FIFO_SERVER,O_RDONLY,0);

       //在此处可以把读程序编译成两个不同版本：阻塞版本及非阻塞版本

       if(fd==-1)

       {

              printf("open for read error\n");

              return;  

       }

       while(1)

       {

 

              memset(r_buf,0,sizeof(r_buf));

              ret_size=read(fd,r_buf,r_size);

              if(ret_size==-1)

                     if(errno==EAGAIN)

                            printf("no data avlaible\n");

              printf("real read bytes %d,string:%s\n",ret_size,r_buf);

              sleep(1);

       }    

       pause();

       unlink(FIFO_SERVER);

}
