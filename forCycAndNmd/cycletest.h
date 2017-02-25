#ifndef CYCLETEST_H
#define CYCLETEST_H

#include "common.h"
#include  <mxml.h> 
#include "otdr.h"
#include  "responed.h"
typedef struct cycletest
{    
   uint32_t  SN;                                                   //SN<MAX_OTDR_SN
   uint32_t  Action;                                               // 1 :start   2:abrot       -1:perStart    -2:perAbort  
   struct
	{
	    uint32_t  SNo;                             // for start and abort  action 
       	    char *    beginTime;                       // for start action 
            char *    period;                          // for start action 
            char *    IP01;                            // for start action 
            char *    IP02;                            // for start action 
            char *    IP03;                            // for start action 
            char *    IP04;                            // for start action 
            char *    IP05;                            // for start action 
            char *    IP06;                            // for start action 
            otdr      paramter;                        // for start action 
   	}Group[MAX_OTDR_SN]; //otdr   paramter[MAX_OTDR_SN];
}cycletest;

responed *  getCycletestParameter(mxml_node_t *root,mxml_node_t *tree,cycletest *cycfpar);
responed *   setCycletestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);  

responed * endCycletestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * endCycle(mxml_node_t *root,mxml_node_t *tree,cycletest *cycfpar);

cycletest * Cycle_Create();
void Cycle_Destory(cycletest *me);

#endif
