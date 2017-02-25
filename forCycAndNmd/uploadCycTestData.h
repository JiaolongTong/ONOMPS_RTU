#ifndef UPLOAD_H
#define UPLOAD_H
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


typedef struct backData backData;
struct backData {

	/***测试参数****/
        otdr * otdrPar;
        char  backIP[16];
       /***门限***/
        float    powerValue;
        float    powerGate;
        int      level;                  //
       /**/
        int SwitchPos;                   //2*2光开关状态
};

backData * backData_Create();
void backData_Destory(backData *me);

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream);
char * setBackAddr(char * backIP);

int upload(backData *bData,int SNo,int CM,int type);

int XMLgenerOpticPowerWarming (char * filename,float powerValue,float powerGate,int level,int SNo,int CM,int type);
int XMLsendProtectSwitchSwitch(char * filename,int SwitchPos,int SNo,int CM,int type);
int XMLgenerNewOTDRData       (char * filename,otdr *Par,int SNo,int CM,int type);

char* base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 
int read_file( char *filename, int *len, char **data );

#endif
