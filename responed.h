#ifndef RESPONED_H
#define RESPONED_H
#include "common.h"
#define MAX_ERROR_SN  32      
#define TYPE_SNo  1
#define TYPE_PNo  2   
/*
0：成功处理命令 	1：设置测试参数非法
3: 命令执行失败	2：设置门限参数非法
4：设置通信参数非法	5：成功处理命令，并等待数据
10：接收文件中存在数据内容 	11：接收到RTU运行时间
12：接收到RTU网络参数	13：切换到保护光缆失败
14：数据库不同步	15：无法找到RTU子单元模块硬件设备 


*/
#define SUCCESS_CODE_OK                   0
#define SUCCESS_CODE_WAIT_BACKDATA        5
#define SUCCESS_CODE_EXIST_BACKDATA       10
#define SUCCESS_CODE_GET_RTUTIME          11
#define SUCCESS_CODE_GET_RTUNETWORK       12    
#define ERROR_CODE_1_TESTPAR_ILLEGAL      1
#define ERROR_CODE_2_SETGATE_ILLEFAL      2
#define ERROR_CODE_3_EXECULTE_FAILED      3
#define ERROR_CODE_4_COMMPAR_ILLEGAL      4
#define ERROR_CODE_13_PROTECT_FAILED      13
#define ERROR_CODE_14_DBMATCH_FAILED      14
#define ERROR_CODE_15_HARDWARE_FAILED     15
                       
typedef struct responed
{    
   int  RespondCode;                                    //错误类型  ErrorSN<MAX_ERROR_SN
   int  ErrorSN;                                        //错误组数(一个设备对应一组)
   int  CM;                                             //局站编号
   int  SNorPN;                                         //发生错误的是SNo还是PNo   1:SNo,2:PNo

   struct ErrorData
	{
	    uint32_t  SNo;                              // 发送错误的光路
            uint32_t  PNo;                              // 组号
            char *    Value;                                          // 回复数据：本地时间、网络参数、光功率数据
            char *    Main_inform;
            char *    Error_inform;                     // 详细信息                                                    
   	}Group[MAX_ERROR_SN];                           
}responed;

responed * Responed_Create();
void Responed_Destory(responed *me);


void RespondMessage_OK(int code);
void RespondMessage_Error(int code ,responed * res);
void RespondMessage_NetworkSegment(void);
void RespondMessage_ReferenceTime(void);
void RespondMessage_AlarmTestData(void);
void NullPossess(void);
#endif
