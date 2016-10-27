#include "opticalpower.h"
#include "process.h"
#include "Communicate.h"
#include "myModbus.h"
opticalpower * OpticPower_Create()
{
    opticalpower * me = (opticalpower *) malloc(sizeof(opticalpower));
    return me;
}
void OpticPower_Destory(opticalpower *me)
{
    free(me);
}
responed * getOpticPowerParameter(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t *CM,*perCMDcode,*SNo;
   opticalpower * power;
   responed *resp; 
   int intCM;
   int uint_a; 
   float powerValue=-100.0;
   char * ValueStr;
   ValueStr =(char *)malloc(sizeof(char)*10);
   power = OpticPower_Create();   
   resp  = Responed_Create();

   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;
            
       }
      else
      {
/**************************解析XML消息***************************************/
            SNo = mxmlFindElement(cmd, tree, "SNo",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (SNo->child->value.text.string, NULL, 0);
	    power->SNo = uint_a;
            CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (CM->child->value.text.string, NULL, 0);
            power->CM   =uint_a;
           if(!setModbus_P())                                                //P
                 exit(EXIT_FAILURE);   
            modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
            powerValue = getOneOpticalValue(mb,power->SNo); 

            freeModbus(mb);                     
            power->value=powerValue;   
            RespondMessage_OpticPowerData(power, 0);      
           if(!setModbus_V())                                                //V
                 exit(EXIT_FAILURE);  
         return resp;
      }
  
}


