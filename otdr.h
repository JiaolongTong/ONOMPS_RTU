#ifndef OTDR_H
#define OTDR_H
#include "tcp-otdr.h"

#include "common.h"

#include <sys/sem.h>

#define BUF_SIZE          (128*1024)              //OTDR测试缓冲区大小

#define BEIJING

#ifndef BEIJING
#define OTDR_IP           "192.168.1.249"
#else
#define OTDR_IP           "192.168.1.180"
#endif             
#define onlyOne_Moudle    1                       // 1 只有一个功能模块  0 不止一个功能模块
 
typedef struct otdr otdr;
struct otdr {
        int      haveParm;                        //是否存在测试参数
        pid_t    masterPID;                       //下发测试任务的控制程序PID
	/***测试参数****/
	uint32_t MeasureLength_m;                 // 测量长度，单位 m
	uint32_t PulseWidth_ns;                   // 光脉冲宽度，单位 ns
	uint32_t Lambda_nm;                       // 工作光波长，单位 nm
	uint32_t MeasureTime_ms;                  // 测量时间，单位 ms
	float    n;                               // 折射率
	float    EndThreshold;                    // 结束门限
	float    NonRelectThreshold;              // 非反射门限
       /***分析门限***/
        int      PS;
        float    AT01;
        float    AT02;
        float    AT03;
        float    AT04;
        float    AT05;
        float    AT06;    
};

/*********构造和析构函数************/
otdr * OTDR_Create();
void   OTDR_Destory(otdr *me);
/*********方法**********************/
int NetworkIdle(int s,char *buf);
otdr *lookupParm(int SNo,int type);
int HostStartMeasure(int sockt,otdr const * me,char * buf);

int ProcessData(char pbuf[], uint32_t len,int * flag,char * fileName);
int OtdrTest(otdr const * me);


union sem_otdr   
{  
    int val;  
    struct semid_ds *buf;  
    unsigned short *arry;  
}; 


int  initOTDRPV();         //OK
int  setOTDRPV();          //OK
void delOTDRPV();          //OK
int  setOTDR_P();          //OK
int  setOTDR_V();          //OK


#endif
