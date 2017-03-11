#ifndef COMMON_H
#define COMMON_H
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



#include <linux/rtc.h>

#include <sys/ioctl.h>

#include <fcntl.h>

#include <getopt.h>

#include <linux/if.h>

#include <arpa/inet.h>

#include <linux/route.h>


#define DEFAULT_ETH "eth0"
#define FALSE    0
#define TRUE     1


#define  FILE_MODE_NAMED  1
#define  FILE_MODE_CYCLE  2
#define  FILE_MODE_ALARM  3

#define  en_MAXSIZE    1024*100                      //100k
#define  en_DATA       "OtdrAllData.hex"             //OTDR test data
#define  RCV_FILE      "/web/cgi-bin/recv.xml"       //Recvice command file  from tomcat server save to this file.

#define  MAX_OTDR_SN   16
#define  MAX_ALARM_AN  8

#define  TIMEFORMAT "%Y-%m-%d %H:%M:%S"

int16_t uint16toint16(uint16_t t);
time_t str2Timestamp(char *str);          // 字符串形式(str): 2016-07-15 21:07:32
time_t getLocalTimestamp();               // 获取本地时间戳
time_t computTime(char * str);             // 按照字符串计算时间（日、时、分）
float htonf(float t);                     //主机浮点字节转换成网络浮点字节
float ntohf(float t);                     //网络浮点字节转换成主机浮点字节
void  uint32tostring(uint32_t lNum,char chWord[]);
int rtc_set_time(char * rtcDev, time_t setTime) ;  //设置RTC时间并更新系统时间
time_t rtc_read_time(char * rtcDev);       //获取RTC时间

int get_ip(unsigned char ip[16]);
int get_ip_netmask(unsigned char ip[16]);

int set_ip(unsigned char ip[16]);
int set_ip_netmask(unsigned char ip[16]);




int Search_Keyword(char *dst_str,char *search_str); //KMP字符串匹配
#endif
