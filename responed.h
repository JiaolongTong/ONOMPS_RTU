#ifndef RESPONED_H
#define RESPONED_H
#include "common.h"
#define MAX_ERROR_SN  32                                
typedef struct responed
{    
   int  RespondCode;                                    //错误类型  ErrorSN<MAX_ERROR_SN
   int  ErrorSN;                                        //错误组数(一个设备对应一组)
   struct ErrorData
	{
	    uint32_t  SNo;                              // 发送错误的设备号
            char *    Error_inform;                     // 详细信息
                                                       
   	}Group[MAX_ERROR_SN];                           
}responed;

responed * Responed_Create();
void Responed_Destory(responed *me);


void RespondMessage_OK(int code);  
void RespondMessage_Error(int code,responed * res);

void RespondMessage_NetworkSegment(void);
           
void RespondMessage_OpticPowerData(void);

void RespondMessage_ReferenceTime(void);
void RespondMessage_AlarmTestData(void);
void NullPossess(void);
#endif
