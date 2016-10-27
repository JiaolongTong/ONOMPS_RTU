#ifndef RESPONED_H
#define RESPONED_H
#include "common.h"
#define MAX_ERROR_SN  32      
#define TYPE_SNo  1
#define TYPE_PNo  2                          
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
