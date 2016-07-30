#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include"cgic.h"
#define BufferLen 1024
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include"cgic.h"
#define BufferLen 1024
int cgiMain(void){
    cgiFilePtr file;
    int    targetFile;
    mode_t    mode;
    // char name[128];
    char fileNameOnServer[64];
    //char contentType[1024];
    char buffer[BufferLen];
    // int size;
    int got;
    cgiHeaderContentType("text/xml");
    /*取得html页面中file元素的值，应该是文件在客户机上的路径名
    if (cgiFormFileName("file", name, sizeof(name)) !=cgiFormSuccess) {
        fprintf(stderr,"could not retrieve filename\n");
        goto FAIL;
    }*/
    // cgiFormFileSize("file", &size);
    //目前文件存在于系统临时文件夹中，通常为/tmp，通过该命令打开临时文件。临时文件的名字与用户文件的名字不同，所以不能通过路径/tmp/userfilename的方式获得文件
    if (cgiFormFileOpen("file", &file) != cgiFormSuccess) {
        printf("<H>could not open the file</H>\n");    //change
        goto FAIL;
    }
    //在当前目录下建立新的文件，第一个参数实际上是路径名，此处的含义是在cgi程序所在的目录（当前目录））建立新文件  
    mode=S_IRWXU|S_IRGRP|S_IROTH; 
    targetFile=open("/web/cgi-bin/test.xml",O_RDWR|O_CREAT|O_TRUNC|O_APPEND,mode);
    if(targetFile<0){
        printf("<H>could not create the new file</H>\n");   //change
        goto    FAIL;
    }
    //从系统临时文件中读出文件内容，并放到刚创建的目标文件中
    while (cgiFormFileRead(file, buffer, BufferLen, &got) ==cgiFormSuccess){
        if(got>0)
            write(targetFile,buffer,got);   
    }
    cgiFormFileClose(file);
    close(targetFile);
    goto    END;
FAIL:
    printf("<H>Failed to upload</H>\n");           //change
    return 1;
END:   
    printf("<H>File has been uploaded</H>\n");
    return 0;
}
