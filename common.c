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
    char * strTo=NULL;
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


int rtc_set_time(char * rtcDev, time_t setTime)   
{
	int retval;
        int fd;
	struct rtc_time rtc_tm;
	struct tm *local;
        struct timeval  tv;

        local=localtime(&setTime);
        
	rtc_tm.tm_mday = local->tm_mday;
	rtc_tm.tm_mon =  local->tm_mon;
	rtc_tm.tm_year = local->tm_year;
	rtc_tm.tm_hour = local->tm_hour;
	rtc_tm.tm_min =  local->tm_min;
	rtc_tm.tm_sec =  local->tm_sec;
	rtc_tm.tm_wday = rtc_tm.tm_yday = rtc_tm.tm_isdst = 0;

	/*set the RTC time/date*/
        fd = open(rtcDev, O_RDWR);
	if (fd ==  -1) {
	   perror(rtcDev);
	   return -1;
	}
	retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
	if (retval == -1) {
		perror("RTC_SET_TIME ioctl");
		return -1;
	}
	close(fd);

        tv.tv_sec=setTime;
        tv.tv_usec = 0;
	if(settimeofday(&tv, (struct timezone *) 0) < 0)
        {
        printf("Set system datetime error!\n");
        return -1;
        }   
	fprintf(stderr, "\n\t\tdate/time is updated to:  %d-%d-%d, %02d:%02d:%02d.\n\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon , rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

       return 0;
}



#define KEYWORD_MAX_LENGTH 100      //设定搜索串的最大长度
int kmp_table[KEYWORD_MAX_LENGTH];  //为搜索串建立kmp表
char prefix_stack[KEYWORD_MAX_LENGTH]; //前缀表达式栈
char suffix_stack[KEYWORD_MAX_LENGTH]; //后缀表达式栈
int keyword_length = 0;  //搜索串的长度
int record_position[KEYWORD_MAX_LENGTH]; //记录与关键字串匹配源串中的位置

/*
*GetMatchValue:获得字符串src的部分匹配值
*/
int GetMatchValue(char *src)
{
    int value = 0;
    int src_len = strlen(src);
    char *begin = src;    //初始化指向字符串第一个字符
    char *end = src + (src_len - 1);  //初始化指向字符串最后一个字符
    int i = 0;
    for(i=0;i<(src_len-1);i++)
    {
        prefix_stack[i] = *begin;
        suffix_stack[i] = *end;
        begin++;
        end--;
    }
    char *p = prefix_stack;
    char *q = suffix_stack + (src_len - 2);  //指向栈中最后一个元素
    int flag = 0;   //用一个标志位来确定后缀栈中到最后一个元素都与前缀栈中的符号匹配
    while(q >= suffix_stack)
    {
        if(*p == *q)
        {
            value++;
            p++;
            flag=1;
        }
        else {
            flag = 0;
        }
        q--;
    }
    if(flag == 0) value = 0;
    return value;
}

/*
*创建搜索字符串的KMP表
*/
int Create_KMP_Table(char *str,int *table)
{
    int i;
    char *dst;
    keyword_length = strlen(str);
    for(i=0;i<keyword_length;i++)
    {
        if(i == 0) {
            table[i] = 0;   //第一个字符无前缀和后缀，所以为0
        }
        else {
            dst = (char*)malloc((i+2));
            if(dst == NULL)
            {
                printf("malloc space error!\n");
                return EXIT_FAILURE;
            }
            strncpy(dst,str,(i+1));   //匹配str的前(i+1)个字符
            dst[i+1] = '\0';    //注意字符串要以'/0'结尾
            table[i] = GetMatchValue(dst); 
            free((void*)dst);    
        }
    }
    return EXIT_SUCCESS;
}
//在目标串dst_str中搜索关键子串search_str,打印出关键字串的位置信息,返回与关键字串匹配的数目
int Search_Keyword(char *dst_str,char *search_str)
{
    char *p = dst_str;
    char *q = search_str;
    char *temp;

    //创建关键字串的KMP表    
    Create_KMP_Table(search_str,kmp_table);
    
    int count = 0;  //记录现在已经匹配的数目
    int k = 0;     //记录与关键字串匹配的字串的数目
    int move = 0;  //当字符串不匹配时，搜索指针移动的位数    

    while(*p != '\0')   //直到搜索到目标串的最后一个字符为止
    {
        temp = p;
        while(*q != '\0')
        {
            if(*q == *temp)
            {
                count++;
                temp++;
                q++;
            }
            else break;
        }
        
        if(count == 0)
            p++;
        else {
            if(count == keyword_length)
            {
                record_position[k++] = (temp-dst_str)-(keyword_length);
            }
            move = count - kmp_table[count-1];
            p += move;
        }

        count = 0;
        q = search_str;
    }
    return k;
}



