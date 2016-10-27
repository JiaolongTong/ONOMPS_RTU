#include "responed.h"
#include "Communicate.h"
#include "common.h"

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

		printf("<Infor>");
                for(i=0;i<ErrorSN;i++)
                {
                   printf("%s,",res->Group[i].Main_inform);                   //i>10待修改  
                }
 		printf("</Infor>");  

		for(i=0;i<ErrorSN;i++){
		 printf("%s PNo:%d %s[SNo:%d]\n",str1,res->Group[i].PNo,res->Group[i].Error_inform,res->Group[i].SNo);
		}
        }else{
		printf("<SNo>");
		for(i=0;i<ErrorSN;i++){
		   uint32tostring((uint32_t)i,str);
		   printf("%d,",res->Group[i].SNo);                   //i>10待修改  
		  
		}
		printf("</SNo>\n");
		printf("<Infor>");
                for(i=0;i<ErrorSN;i++)
                {
                   printf("%s,",res->Group[i].Main_inform);                   //i>10待修改  
                }
 		printf("</Infor>");               
		for(i=0;i<ErrorSN;i++){

		 printf("%s SNo:%d %s\n",str1,res->Group[i].SNo,res->Group[i].Error_inform);
                }	
        }
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

