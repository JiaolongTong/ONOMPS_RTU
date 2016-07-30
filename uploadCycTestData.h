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
#define  TOMCAT_SERVER    "http://192.168.0.120:8080/Fiber/TomCat"
#define  BOA_SERVER       "http://192.168.0.115:5000/cgi-bin/BoaCom.cgi?value=SetNamedTestSegment" 
#define  BACKFILE     "back.xml"  
#define  RTUSENDFILE  "send.xml"
#define  en_ORDRDATA  "OtdrAllData.hex"
#define  en_MAXSIZE    1024*100


int upload(int SNo,otdr * Par);
size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream);
int XMLgenerLineFaultWarming(char * filename,char * types); 
int XMLgenerOpticPowerWarming (char * filename,char * types); 
int XMLgenerStartCableProtection (char * filename,char * types); 
int XMLgenerNewOTDRData (char * filename,char * types,int SNo,otdr *Par);



char* base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 
int read_file( char *filename, int *len, char **data );

