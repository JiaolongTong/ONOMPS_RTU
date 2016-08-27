#include "responed.h"
#include "Communicate.h"
#include "common.h"
/**************************************************************
                Responed Code
0：成功处理命令 	1：设置测试参数非法
3: 命令无效	        2：设置门限参数非法
4：设置通信参数非法	5：成功处理命令，并等待数据 (ignore)
10：接收到测试数据文件	11：接收到RTU运行时间
12：接收到RTU网络参数	13：成功切换到保护光缆

**************************************************************/
responed * Responed_Create()
{
	responed * me = (responed *) malloc(sizeof(responed));
        return me;	
}
void Responed_Destory(responed *me)
{
	free(me);
}


void RespondMessage_OK(int code)
{
        char * str;
        str = getCommendString(code);
	printf("<RespondCode>0</RespondCode>\n");
        printf("<Data>\n");
        printf("	%s Successful\n" ,str);
        printf("</Data>\n");	
}
void RespondMessage_Error(int code ,responed * res)
{

    int ErrorSN=0,i=0;
    char * str;
    char * str1;
    str1 = getCommendString(code);
    str = (char *) malloc (sizeof(char)*5);
	printf("<RespondCode>%d</RespondCode>\n",res->RespondCode);
        printf("<Data>\n");
        ErrorSN=res->ErrorSN;
        printf("<ErrorSN>%d</ErrorSN>\n",ErrorSN);
        if(res->SNorPN==TYPE_PNo){
		printf("<PNo>");
		for(i=0;i<ErrorSN;i++){
		   uint32tostring((uint32_t)i,str);
		   printf("%d,",res->Group[i].PNo);                   //i>10待修改  
		}
		printf("</PNo>\n");
		for(i=0;i<ErrorSN;i++){
		 printf("%s PNo:%d %s\n",str1,res->Group[i].PNo,res->Group[i].Error_inform);
		}
        }else{
		printf("<SNo>");
		for(i=0;i<ErrorSN;i++){
		   uint32tostring((uint32_t)i,str);
		   printf("%d,",res->Group[i].SNo);                   //i>10待修改  
		  
		}
		printf("</SNo>\n");
		for(i=0;i<ErrorSN;i++){
		 printf("%s SNo:%d %s\n",str1,res->Group[i].SNo,res->Group[i].Error_inform);
                }	
        }
        printf("</Data>\n");
        
}
void RespondMessage_OpticPowerData(void)
{
	printf("<RespondCode>0</RespondCode>\n");
        printf("<Data>\n");
        printf("</Data>\n");	      
}

void RespondMessage_NetworkSegment(void)
{
	printf("<RespondCode>12</RespondCode>\n");
        printf("<Data>\n");	
        printf("</Data>\n");	      
}
void RespondMessage_ReferenceTime(void)
{
	printf("<RespondCode>11</RespondCode>\n");
        printf("<Data>\n");	
        printf("</Data>\n");	      
}
void RespondMessage_AlarmTestData(void)
{
	printf("<RespondCode>10</RespondCode>\n");
        printf("<Data>\n");
        printf("</Data>\n");	      
}

void NullPossess(void)
{
       printf("<RespondCode>3</RespondCode>\n");
       printf("<Data>\n");
       printf("</Data>\n");	
}

