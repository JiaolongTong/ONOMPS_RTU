#ifndef OPTICALPROTECT_H
#define OPTICALPROTECT_H
#include "common.h"
#include  <mxml.h> 
#include  "responed.h"

typedef struct opticalprotect{
  
   uint32_t  PN;                                                   //SN<MAX_OTDR_SN
   uint32_t  Action;                                               // 1 :start   2:abrot       -1:perStart    -2:perAbort  
   struct
	{
            uint32_t  PNo;
	    uint32_t  SNoA;                             // for start and abort  action 
            uint32_t  SNoB;                             // for start and abort  action 
   	}Group[MAX_OTDR_SN/2];                          //otdr   paramter[MAX_OTDR_SN];
}opticalprotect;




opticalprotect * OpticalProtect_Create();
void OpticalProtect_Destory(opticalprotect *me);

void getOpticalProtectParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar);
responed *  setOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

responed *  endOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

//int ifProtectMatch(int SNoA,int SNoB,sql *mysql);

responed *  requestProtectSwitch(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
void getProtectSwitchParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar);
#endif
