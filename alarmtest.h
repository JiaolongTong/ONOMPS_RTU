#ifndef ALARMTEST_H
#define ALARMTEST_H

#include "common.h"
#include  <mxml.h> 
#include  "otdr.h"
#include  "responed.h"
typedef struct gate{
    float     AT01;                                // 全程传输损耗门限(dB)
    float     AT02;                                // 光学长度门限(m)
    float     AT03;                                // 接头损耗门限(dB)
    float     AT04;                                // S、R最大离散发射门限(dB)
    float     AT05;                                // 两接头间光纤衰减系数门限(db)
    float     AT06;                                // 光功率参考门限(dBm)
}gate;
typedef struct alarmtest{    
   uint32_t  AN;                                                    // 告警组数
   uint32_t  Action;                                                // 指令处理状态  1 :start       -1:perStart    
   struct{
            uint32_t  ANo;                                          // 告警组序号
            uint32_t  ASN;                                          // 告警组中光端口数量
            struct{	    
              	    uint32_t  ASNo;                                 // 光端口号
		    otdr      paramter;                             // 告警数据库本地测试参数
                    gate      alarmGate;                            // 告警门限
                    int       PS;                                   // 是否使用优化参数  0:使用 1:不使用
                    int       fibreType;                            // 光路类型，0：备纤。1：在纤( 在保护模式下起作用 )。默认0
                    int       protectFlag;                          // 是否存在保护纤，0：不存在，1：存在
		    char *    IP01;                                 // 回传地址1
		    char *    IP02;                                 // 回传地址2
		    char *    IP03;                                 // 回传地址3
		    char *    IP04;                                 // 回传地址4
		    char *    IP05;                                 // 回传地址5
		    char *    IP06;                                 // 回传地址6
                    char *    T3;                                   // 二次告警间隔
                    char *    T4;                                   // 通信故障重发间隔
           }portGroup[8];                                           // 每个模块最多8个光开关端口       
   }levelGroup[MAX_ALARM_AN];                      
}alarmtest;


typedef struct cancelAlarmtest
{    
   uint32_t  SN;                                                   // SN<MAX_OTDR_SN
   uint32_t  Action;                                               // 2:abrot     -2:perAbort  
   struct
	{
	    uint32_t  SNo;                                         // for start and abort  action 
            uint32_t  Level;
   	}Group[MAX_OTDR_SN];                                       // 最多取消64组    MAX_OTDR_SN=64
}cancelAlarmtest;

typedef struct alarminfom{
         uint32_t CM;
         uint32_t AI;
}alarminfom;



void getAlarmtestParameter(mxml_node_t *root,mxml_node_t *tree,alarmtest *alarmpar);
responed * setAlarmtestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);  
responed * endAlarmtestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

responed * endAlarmInfo(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * setAlarmInformation(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);  

alarminfom * AlarmInfo_Create();
void AlarmInfo_Destory(alarminfom *me);

alarmtest  * Alarm_Create();
void Alarm_Destory(alarmtest *me);

cancelAlarmtest  * endAlarm_Create();
void endAlarm_Destory(cancelAlarmtest *me);

#endif
