#include "rtuIformation.h"

rtuInform * RTU_Create()
{
    rtuInform * me = (rtuInform *) malloc(sizeof(rtuInform));
    return me;
}
void RTU_Destory(rtuInform *me)
{
    free(me);
}



responed * setReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*T8,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   int intCM;
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    T8 = mxmlFindElement(cmd, tree, "T8",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuRefenceTime= strtoul(T8->child->value.text.string, NULL, 0); 	  
            printf("------setAlarm------\n");    
            printf("rtuCM=%d\n",rtuReferenceTime->rtuCM); 
            printf("StampTime=%d\n",rtuReferenceTime->rtuRefenceTime);
            time_t rawtime =(time_t)rtuReferenceTime->rtuRefenceTime;
            rtc_set_time("/dev/rtc0",rawtime);
            char * timE =ctime(&rawtime);
            printf("System Time:%s\n",timE);
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;
}

responed * requestReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*T8,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   int intCM;
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    T8 = mxmlFindElement(cmd, tree, "T8",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuRefenceTime= strtoul(T8->child->value.text.string, NULL, 0); 	  
            time_t rawtime = getLocalTimestamp() ;
            char * timE =ctime(&rawtime);
 	    printf("<RespondCode>11</RespondCode>\n");
            printf("<Data>\n");
            printf("<CMDcode>514</CMDcode>\n");
            printf("	<R>*</R>\n");
            printf("	<CM>%d</CM>\n",rtuReferenceTime->rtuCM);
            printf("	<T8>%ld<T8>\n" ,rawtime);
            printf("</Data>\n");	
            printf("System Time:%s\n",timE);
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   
}

responed * setRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*perCMDcode;
   rtuInform *rtuMode;
   responed *resp; 
   int intCM;
  
   rtuMode = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    mxml_node_t *KX,*MN,*KNo,*Type;
	    int  intKNo,intType,intMN,i;
            char strKX[3]="Kx";

	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

            MN =mxmlFindElement(cmd, tree, "MN",NULL, NULL,MXML_DESCEND);
            intMN =  strtoul(MN->child->value.text.string, NULL, 0);
            rtuMode->sumModule =intMN;                                            //RTU中子单元数量


            for(i=0;i<MAXSUBMODULE;i++)
            {
                  rtuMode->rtuMode[i]=0;                                         //获取本地已有模块信息(需要查询数据库)  
            }

            for (i=0;i<intMN;i++)                                                //
            { 
		      strKX[1]=i+0x31;
		      KX     = mxmlFindElement(cmd, tree, strKX ,NULL, NULL,MXML_DESCEND); 
                      KNo    = mxmlFindElement(KX,tree,"KNo",NULL, NULL,MXML_DESCEND); 
                      intKNo =  strtoul(KNo->child->value.text.string, NULL, 0);  

                      Type  = mxmlFindElement(KX,tree,"Type",NULL, NULL,MXML_DESCEND); 
                      intType =  strtoul(Type->child->value.text.string, NULL, 0);  

                      rtuMode->rtuMode[intKNo-1]=intType;
                            
            }


            printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
            for(i=0;i<MAXSUBMODULE;i++)
            {
                  printf("子单元模块:%d   类型:%d\n",i+1,rtuMode->rtuMode[i]);  
            }
/**************************************************************************/
       }  
   RTU_Destory(rtuMode);
   return resp; 
}

responed * setNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{

}



