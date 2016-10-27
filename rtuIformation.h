#ifndef RTUINFORMATION_H
#define RTUINFORNATION_H
#include <mxml.h> 
#include "common.h"
#include "Communicate.h"
#include  "responed.h"

#define MAXSUBMODULE  8

typedef struct rtuInform
{
    uint32_t rtuCM;
    time_t   rtuRefenceTime;	 
    char *   rtuIP;
    char *   rtuGateway;
    char *   rtuNetmask;
    uint32_t sumModule;
    char     rtuMode[MAXSUBMODULE];
}rtuInform;


responed * setReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * requestReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * setNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * setRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

rtuInform * RTU_Create();
void RTU_Destory(rtuInform *me);

#endif
