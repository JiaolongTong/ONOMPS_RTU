#ifndef OPTICALPOWER_H
#define OPTICALPOWER_H
#include <mxml.h> 
#include "common.h"
#include "responed.h"
typedef struct opticalpower
{
    uint32_t SNo;
    uint32_t CM;
    float    value;
}opticalpower;

responed * getOpticPowerParameter(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

opticalpower * OpticPower_Create();
void OpticPower_Destory(opticalpower *me);

#endif
