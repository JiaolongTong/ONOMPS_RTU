#include "common.h"
void uint32tostring(uint32_t lNum,char chWord[])

{
   uint32_t i=0,j;
   char chTemp;
   while(lNum!=0)                      /*依次取整数的末位，存入chWord */
   {
     chWord[i]='0'+lNum%10;           /*转换为数字的ASCII码*/
     i++;
     lNum=lNum/10;  
   }  
   chWord[i]='\0';                    /*字符串最后一位加'\0'*/   
   for(j=0;j<i/2;j++)                  /*将字符串转置*/
   {
   chTemp=chWord[j];
   chWord[j]=chWord[i-1-j];
   chWord[i-1-j]=chTemp;
   }
}

// tcp-otdr.c
// Access OTDR using TCP

float htonf(float t)                      //主机浮点字节转换成网络浮点字节
	{
		uint32_t ux;
		float x = t;
		ux = *(uint32_t*)&x;
		ux = htonl(ux);
		return *(float*)&ux;
	}

float ntohf(float t)                     //网络浮点字节转换成主机浮点字节
	{
		uint32_t ux;
		float x = t;
		ux = *(uint32_t*)&x;
		ux = ntohl(ux);
		return *(float*)&ux;
	}

time_t str2Timestamp(char *str)          // 字符串形式(str): 20160715210732
{  
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));  
    int i;
    char * strTo;
    char year[5],mouth[3],day[3],h[3],m[3],s[3];
    for(i=0;i<4;i++)
    {
        year[i] = str[i];
    }
    year[i] ='\0';
    for(i=0;i<2;i++)
    {
        mouth[i] = str[i+4];
    }
     mouth[i] ='\0';

    for(i=0;i<2;i++)
    {
        day[i] = str[i+4+2];
    }
     day[i] ='\0';

    for(i=0;i<2;i++)
    {
        h[i] = str[i+4+2+2];
    }
     h[i] ='\0';

    for(i=0;i<2;i++)
    {
        m[i] = str[i+4+2+2+2];
    }
     m[i] ='\0';

    for(i=0;i<2;i++)
    {
        s[i] = str[i+4+2+2+2+2];
    }
     s[i] ='\0';
    
    strTo=(char *) malloc (sizeof(char)*30);
    sprintf(strTo,"%s-%s-%s %s:%s:%s",year,mouth,day,h,m,s);
    printf("DATE:%s\n",strTo);
    strptime(strTo,TIMEFORMAT,tmp_time);  
    time_t t = mktime(tmp_time);  
    //printf("zhuTimesstamp:%ld\n",t); 
    free(strTo);
    free(tmp_time);  
    return t;  
}

time_t computTime(char * str)           //str:日、时、分各2字节
{
   int i;
   char day[3],h[3],m[3];
   
    for(i=0;i<2;i++)
    {
        day[i] = str[i];
    }
     day[i] ='\0';

    for(i=0;i<2;i++)
    {
        h[i] = str[i+2];
    }
     h[i] ='\0';

    for(i=0;i<2;i++)
    {
        m[i] = str[i+2+2];
    }
     m[i] ='\0';
 
     time_t res=0;
 
     printf("Day:%s Hour:%s  Munitis:%s\n",day,h,m);
    // time_t  day_t,h_t,m_t;
     //day_t = strtoul (P03->child->value.text.string, NULL, 0); 
     res =(time_t)(atoi(m)*60 + atoi(h)*60*60 + atoi(day)*60*60*24);
     return(res);
    
}
time_t getLocalTimestamp()               // 获取本地时间戳
{
     time_t timep;
     struct tm *p;  
     char s[100];
     time(&timep);
     p=gmtime(&timep);  
     strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);  
     //printf("NOW :%s\n", s);  
     //printf("nowTimesstamp:%ld\n",timep);
     return(timep);
}



