#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include <netinet/in.h>

#include <arpa/inet.h>
#include <errno.h>
#include <time.h>


#include <sys/stat.h>
#include <fcntl.h>
void DecodeAndProcessData(char *data,int len);
int main(void)  
    {  
	  char *pRequestMethod;
          char *data;
          char *postlength;
	  int i,fd,ContentLength;
          char InputBuffer[4096*100];
          char *getbuf; 
          char x; 
          printf("Content-type:text/xml\n\n"); //这句一定要加上  
          printf("<?xml version='1.0' encoding='UTF-8'?>\n"); 
          printf("<top>");
          printf("<hello> Hello World</hello>\n");  
          pRequestMethod = getenv("REQUEST_METHOD");    
          if(pRequestMethod==NULL)
           {   
               printf("<Error>Get Request_method Error</Error>");     
               return   0;    
           } 
	  else
          {
               printf("<Method> %s </Method>\n",pRequestMethod);
          }

          if (strcmp(pRequestMethod,"GET")==0)   {    
                data   =   getenv("QUERY_STRING");                                            //get From data from QUERY_STRING
                      if   (data!=NULL)   {
                              //InputBuffer = malloc(sizeof(char)*1024*4);    
                              strncpy(InputBuffer,data,sizeof(InputBuffer));    
                              printf("<GetData> %s </GetData>\n",data);
                              DecodeAndProcessData(InputBuffer,strlen(InputBuffer));          //Save data  
                              //free(InputBuffer);
                      }
          } else
          if   (strcmp(pRequestMethod,"POST")==0)   {    
                 postlength=getenv("CONTENT_LENGTH");                                         //Get Length from env-value  
                 printf("<PostLen> %s </PostLen>\n",postlength);                 
		 data   =   getenv("QUERY_STRING"); 
                 printf("<PostString> %s </PostString>\n",data);
                      if(postlength!=NULL){    
                              ContentLength=atoi(postlength);   
                             // InputBuffer = malloc(ContentLength);  
                              printf("<len> len=%d </len>\n",ContentLength);
                      }
                      else   {    
                              ContentLength   =   0;  
                              printf("<Error> Lack of data </Error>\n");
                      }    
                      if   (ContentLength>sizeof(InputBuffer)-1)   {    
                              ContentLength   =   sizeof(InputBuffer)-1;    
                      }    
                      i   =   0;    
                      while   (i<ContentLength)   {                                          //get Form data from stdin
                              x   =   fgetc(stdin);    
                              if   (x==EOF)   break;    
                              InputBuffer[i++]   =   x;    
                      }    
                      InputBuffer[i]  =   '\0';    
                      ContentLength   =   i;    
                      DecodeAndProcessData(InputBuffer,ContentLength);                       //Save data
                     // free(InputBuffer);
              }        
	 
          printf("</top>");
         return 0;  
    }  

void DecodeAndProcessData(char *data,int len)
{
         int fd;
         fd=open("/web/cgi-bin/test.xml",O_RDWR|O_CREAT|O_TRUNC|O_APPEND,S_IRWXU|S_IRGRP|S_IROTH);
         write(fd,data,len);   
         close(fd);
}



