#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>  

typedef struct configure{
       int SUM;
       struct {
       char  key[20];
       char  value[20];
       }recode[20];
}configure;
char *basename(const char *path)
{
        register const char *s;
        register const char *p;

        p = s = path;

        while (*s) {
            if (*s++ == '/') {
                p = s;
            }
        }

        return (char *) p;
    }
int getPIDbyName(const char* process_name, pid_t pid_list[], int list_size)
    {
        #define  MAX_BUF_SIZE       256
        DIR *dir;
        struct dirent *next;
        int count=0;
        pid_t pid;
        FILE *fp;
        char *base_pname = NULL;
        char *base_fname = NULL;
        char cmdline[MAX_BUF_SIZE];
        char path[MAX_BUF_SIZE];

        if(process_name == NULL || pid_list == NULL)
            return -EINVAL;

        base_pname = basename(process_name);
        if(strlen(base_pname) <= 0)
            return -EINVAL;

        dir = opendir("/proc");
        if (!dir)
        {
            return -EIO;
        }
        while ((next = readdir(dir)) != NULL) {
            if (!isdigit(*next->d_name))
                continue;
            pid = strtol(next->d_name, NULL, 0);
            sprintf(path, "/proc/%u/cmdline", pid);
            fp = fopen(path, "r");
            if(fp == NULL)
                continue;

            memset(cmdline, 0, sizeof(cmdline));
            if(fread(cmdline, MAX_BUF_SIZE - 1, 1, fp) < 0){
                fclose(fp);
                continue;
            }
            fclose(fp);
            base_fname = basename(cmdline);

            if (strcmp(base_fname, base_pname) == 0 )
            {
                if(count >= list_size){
                    break;
                }else{
                    pid_list[count] = pid;
                    count++;
                }
            }
        }
        closedir(dir) ;
        return count;
 }
void readFile(configure *cfg,char * configFile ) {
        char lineRecod[10][40];
        char tmp[2],name[20],value[20];
        int  i=0,j=0,k=0,hang=0,keyFlag=0;
        int fd = open(configFile, O_RDONLY);
        if(fd == -1) {
                printf("error is %s\n", strerror(errno));
                return;
        } 
        memset(tmp, '\0', sizeof(tmp));
        while(read(fd, tmp, 1 ) > 0) {
             if(tmp[0] !='\n'){
                  lineRecod[i][j++] = tmp[0];
             }else{ 
                  lineRecod[i++][j] ='\0';
                  j=0;
             } 
             memset(tmp, 0, sizeof(tmp));
        }
        cfg->SUM=i;
        
        for(i=0;i<cfg->SUM;i++){
            j=0;
            while(lineRecod[i][j]!=' '){
               name[j]=lineRecod[i][j];
               name[j+1]='\0';
               j++;
            }
            k=0;
            j++;
            while(lineRecod[i][j+k]!='\0'){
               value[k]=lineRecod[i][j+k];
               value[k+1]='\0';   
               k++;  
            }
            printf("SuanfaA :Recode key:%s value:%s\n",name,value);
            strcpy(cfg->recode[i].key,name);
            strcpy(cfg->recode[i].value,value);
        }

/*  
        keyFlag=1;
        j=0;i=0;k=0;
        while(1) {   
             if(keyFlag==1){
                   if(lineRecod[k][i] == ' '){
                       name[i+1] = '\0'; 
                       keyFlag=0;
                       i++;
                   } 
                   else{
                     name[i] = lineRecod[k][i]; 
                     i++;                  
                   }
             }else{
                  if(lineRecod[k][i+j] =='\0'){  
                      value[j] = '\0';
                      keyFlag=1; 
                      i=0;
                      j=0;
                      strcpy(cfg->recode[k].key,name);
                      strcpy(cfg->recode[k].value,value);
                      if(k++==cfg->SUM-1)break;
                  } 
                  else{
                      value[j] = lineRecod[k][i+j];
                      j++ ;
                  }
                  
             }              
        }
*/
        close(fd);
}

long int count=0;
int  flag=0;
void time_out(int signo){
    switch (signo){
        case SIGVTALRM:
            //printf("运行自检程序....%d次\n",count++);
            flag = 1;
            break;
    }
    return;
}

int checkType(char *process){
     if (strcmp(process,"eth0")==0) return 1;
     if (strcmp(process,"eth1")==0) return 2; 
     if (strcmp(process,"ServerIP")==0) return 3;
     if (strcmp(process,"otdrMain")==0) return 4;   
     if (strcmp(process,"cycMain")==0) return 5;   
     if (strcmp(process,"alarmMain")==0) return 6;   
     if (strcmp(process,"ProtectMasterMain")==0) return 7;   
     if (strcmp(process,"ProtectSlaveMain")==0) return 8;  
}

void ifInback(char *state,char * processName){
     char processALL[40];
     int  ret;
     pid_t processPID[2];
     if (strcmp(state,"y")==0 ||  strcmp(state,"Y")==0 ){
          sprintf(processALL,"/web/cgi-bin/%s",processName);               
	  ret = getPIDbyName(processALL, processPID, 1);  
          if(ret>0){
                //printf("进程:%s 运行正常!\n",processName);
                return ;
          }
          else{
              printf("进程:%s 意外退出，重启RTU\n",processName);
              execl("/web/cgi-bin/reboot.sh","reboot.sh");  
          }
     } 
    
}

int main(void)
{
   int i=0,ckeckType=0;
   configure *cfg =NULL;
   struct itimerval value, ovalue;  
   cfg= (configure *)malloc(sizeof(configure));
   readFile(cfg,"fiberMointor.conf");
   free(cfg);
   cfg==NULL;
   while(1){
     sleep(60*1);
	   cfg= (configure *)malloc(sizeof(configure));
	   readFile(cfg,"fiberMointor.conf");          
	   for(i=0;i<cfg->SUM;i++){  
	       switch(checkType(cfg->recode[i].key)){
                  case 1 :             
                  case 2 :
                  case 3 :break;
                  case 4 :
                  case 5 :
                  case 6 :
                  case 7 :
                  case 8 :ifInback(cfg->recode[i].value,cfg->recode[i].key); break;                 
                  default : break;
               }
	   }
	   free(cfg);
	   cfg==NULL;
   }
}
