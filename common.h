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
#define  FILE_MODE_NAMED  1
#define  FILE_MODE_CYCLE  2
#define  FILE_MODE_ALARM  3

#define  en_MAXSIZE    1024*100                      //100k
#define  en_DATA       "OtdrAllData.hex"             //OTDR test data
#define  RCV_FILE      "/web/cgi-bin/recv.xml"       //Recvice command file  from tomcat server save to this file.

#define  MAX_OTDR_SN   16

#define  TIMEFORMAT "%Y-%m-%d %H:%M:%S"


time_t str2Timestamp(char *str);          // 字符串形式(str): 2016-07-15 21:07:32
time_t getLocalTimestamp();               // 获取本地时间戳
time_t computTime(char * str);             // 按照字符串计算时间（日、时、分）
float htonf(float t);                     //主机浮点字节转换成网络浮点字节
float ntohf(float t);                     //网络浮点字节转换成主机浮点字节
void  uint32tostring(uint32_t lNum,char chWord[]);
#endif
