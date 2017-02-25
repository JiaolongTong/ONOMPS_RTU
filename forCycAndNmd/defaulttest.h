#ifndef DEFAULTTEST_H
#define DEFALLTTEST_H
#include "otdr.h"
#include "common.h"
#include  <mxml.h> 
#include  "responed.h"

typedef struct defaulttest
{    
   uint32_t  SN;                                                    //SN<MAX_OTDR_SN
   struct
	{
	    uint32_t  SNo;
       	    otdr      paramter;
   	}Group[MAX_OTDR_SN];
                                                                    //otdr   paramter[MAX_OTDR_SN];
}defaulttest;

void  getDefaultParameter(mxml_node_t *root,mxml_node_t *tree,defaulttest *defpar);
responed * setDefaultTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode); 

defaulttest * DefaultP_Create();
void DefaultP_Destory(defaulttest *me);
#endif
