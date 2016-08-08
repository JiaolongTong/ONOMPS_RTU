#ifndef OTDR_H
#define OTDR_H
#include "tcp-otdr.h"
#include "common.h"
#define BUF_SIZE          (128*1024)       //OTDR测试缓冲区大小
#define OTDR_IP           "192.168.0.180"
typedef struct otdr otdr;
struct otdr {
	/***属性****/
	uint32_t MeasureLength_m;              // 测量长度，单位 m
	uint32_t PulseWidth_ns;                // 光脉冲宽度，单位 ns
	uint32_t Lambda_nm;                    // 工作光波长，单位 nm
	uint32_t MeasureTime_ms;               // 测量时间，单位 ms
	float    n;                               // 折射率
	float    EndThreshold;                    // 结束门限
	float    NonRelectThreshold;              // 非反射门限
};

/*********构造和析构函数************/
otdr * OTDR_Create();
void OTDR_Destory(otdr *me);
/*********方法**********************/
int NetworkIdle(int s,char *buf);
otdr *lookupParm(int SNo);
int HostStartMeasure(int sockt,otdr const * me,char * buf);
int ProcessData(char pbuf[], uint32_t len,int * flag);
int OtdrTest(otdr const * me);

#endif
