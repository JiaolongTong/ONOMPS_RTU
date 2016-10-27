#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>  
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
#include "otdr.h"

#define  TOMCAT_SERVER    "http://192.168.0.150:8080/fiberMonitor/TomCat"
#define  BOA_SERVER       "http://192.168.0.107:5000/cgi-bin/BoaCom.cgi?value=SetNamedTestSegment" 
#define  BACKFILE     "back.xml"  
#define  RTUSENDFILE  "send.xml"
#define  en_ORDRDATA  "OtdrAllData.hex"
#define  en_MAXSIZE    1024*100

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream);
int upload(int SNo,int CM,otdr *Par,int type);
int XMLgenerLineFaultWarming(char * filename,int type);
int XMLgenerOpticPowerWarming (char * filename,int SNo,int CM,otdr *Par,int type);
int XMLgenerNewOTDRData (char * filename,int SNo,int CM,otdr *Par,int type);

char* base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 
int read_file( char *filename, int *len, char **data );

