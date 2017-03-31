#ifndef OPTICALPROTECT_H
#define OPTICALPROTECT_H
#include "common.h"
#include  <mxml.h> 
#include  "responed.h"

#define STATUS_AUTO_PROTECT 1
#define STATUS_PerSTATUS_PROTECT -1
#define STATUS_WAIT_GATE    0


typedef struct opticalprotect{

   uint32_t  CM;
   uint32_t  CLP; 
   uint32_t  PN;                                                   //SN<MAX_OTDR_SN
   uint32_t  Action;                                               // 1 :start   2:abrot       -1:perStart    -2:perAbort  
   struct
	{

            uint32_t  ConnectPos;                                  //主从站连接关系
            uint32_t  ModNo;                                       //从站RTU上与之配对模块编号
            char   *  IP;                                          //从站RTU与之配对IP地址

            uint32_t  PNo;                                         //主站下行光路保护组编号
	    uint32_t  SNoA;                                        //主站下行在纤（备纤）光路号
            uint32_t  SNoB;                                        //主站下行备纤（在纤）光路号
            uint32_t  SwitchPos;                                   //主站1*2光开关状态                     

            uint32_t  sPNo;                                        //主站上行光路保护组编号
	    uint32_t  sSNoA;                                       //主站上行在纤（备纤）光路号
            uint32_t  sSNoB;                                       //主站上行在纤（备纤）光路号
            uint32_t  sSwitchPos;                                  //从站1*2光开关状态  

            float     sPowerGateA;                                 //从站A类光路光功率门限（小）
            float     sPowerGateB;                                 //从站B类光路光功率门限（大）

    	}Group[MAX_OTDR_SN/2];                           
}opticalprotect;




opticalprotect * OpticalProtect_Create();
void OpticalProtect_Destory(opticalprotect *me);

void getOpticalProtectParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar);
responed *  setOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed *  endOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed *  requestProtectSwitch(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
void getProtectSwitchParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar);
#endif
