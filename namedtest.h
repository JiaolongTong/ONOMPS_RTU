#ifndef NAMEDTEST_H
#define NAMEDTEST_H
#include <mxml.h> 
#include "common.h"
#include "otdr.h"
#include "Communicate.h"
typedef struct namedtest
{
    uint32_t SNo;
    uint32_t PS;	 
    otdr     paramter;
}namedtest;

void getNamedParameter(mxml_node_t *root,mxml_node_t *tree, namedtest *namedpar);
int SetNamedTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode);

namedtest * NamedP_Create();
void NamedP_Destory(namedtest *me);

#endif
