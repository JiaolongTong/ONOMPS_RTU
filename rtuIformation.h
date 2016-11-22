#ifndef RTUINFORMATION_H
#define RTUINFORNATION_H
#include <mxml.h> 
#include "common.h"
#include "Communicate.h"
#include  "responed.h"

#define MAXSUBMODULE  8
#define MAXRTUPORT    64

typedef struct rtuInform
{
    uint32_t rtuCM;
    uint32_t rtuCLP;
    time_t   rtuRefenceTime;	 
    char *   rtuIP;
    char *   rtuGateway;
    char *   rtuNetmask;
    uint32_t sumModule;
    uint32_t sumPort;
    char     rtuMode[MAXSUBMODULE];            //模块类型有五种：0，不存在，1，在纤（不带光功率），2备纤，3混合，4在纤（带光功率）,  一个模块对应一个数组下标
    char     rtuPort[MAXRTUPORT];
}rtuInform;                                    //光纤类型+1（备纤：0+1，在线纤：1+1）。默认0+1


responed * setReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * requestReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

responed * setNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * requestNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);


responed * setRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * cancelRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

responed * setRTUPort(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);
responed * cancelRTUPort(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);


rtuInform * RTU_Create();
void RTU_Destory(rtuInform *me);

#endif
