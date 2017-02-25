/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include <modbus/modbus.h>
#include "myModbus.h"
#include "common.h"

int modbus_sem_id; 
int main(int argc,char **argv)
{

    int rc;
    modbus_t *mb;


    struct timeval t_start,t_end;
    float elapsed;

    mb=newModBus_TCP_Client("192.168.0.118");

    gettimeofday(&t_start,NULL);     
    if(rc=createSlaverProtectModule(mb,1,2,3)<0){
         printf("Error to create new slaver protect module!\n");   
    }else{
         printf("Create slaver protect module successful!\n");
    }
    gettimeofday(&t_end,NULL);     
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("createSlaverProtectModule:Spend time:%f ms\n",elapsed/1000.0);

    gettimeofday(&t_start,NULL);    
    if(rc=setSlaverProtectGroup(mb,1,2,6,0x10,0x00)<0){
         printf("Error to set slaver protect group!\n");   
    }else{
         printf("set slaver protect group successful!\n");
    }
    gettimeofday(&t_end,NULL);  
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("setSlaverProtectGroup:Spend time:%f ms\n",elapsed/1000.0); 

    gettimeofday(&t_start,NULL);    
    if(rc=setSlaverProtectGate(mb,1,-30.1,-30.2)<0){
         printf("Error to set slaver protect powergate!\n");   
    }else{
         printf("set slaver protect powergate successful!\n");
    }
    gettimeofday(&t_end,NULL);  
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("setSlaverProtectGate:Spend time:%f ms\n",elapsed/1000.0); 

/*
    gettimeofday(&t_start,NULL);    
    if(rc=deleteSlaverProtectModule(mb,1)<0){
         printf("Error to delete slaver protect module!\n");   
    }else{
         printf("delete slaver protect module successful!\n");
    }
    gettimeofday(&t_end,NULL);  
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("deleteSlaverProtectModule:Spend time:%f ms\n",elapsed/1000.0); 

    gettimeofday(&t_start,NULL);    
    if(rc=doSlaverProtectSwitch(mb,1,0x10)<0){
         printf("Error to do slaver protect switch!\n");   
    }else{
         printf("do slaver protect switch successful!\n");
    }
    gettimeofday(&t_end,NULL);  
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("doSlaverProtectSwitch:Spend time:%f ms\n",elapsed/1000.0); 
*/
    gettimeofday(&t_start,NULL);    
    slaverModuleInformatin * slaverModule=NULL;
    slaverModule=newSlaverModule();
    slaverModule->ModNo=1;
    if(rc=getSlaverModuleInformation(mb,slaverModule)<0){
        printf("Error to get slaver module information!\n");   
    }else{
        printf("get slaver module information successful!\n"); 
    }
    gettimeofday(&t_end,NULL);  
    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("getSlaverModuleInformation:Spend time:%f ms\n",elapsed/1000.0); 
    
     printf("ModNo:%d\n",slaverModule->ModNo);
     printf("SNoA:%d\n",slaverModule->detail.SNoA);
     printf("SNoB:%d\n",slaverModule->detail.SNoB);
     printf("SwitchPosA:%d\n",slaverModule->detail.SwitchPosA);
     printf("SwitchPosB:%d\n",slaverModule->detail.SwitchPosB);
     printf("powerValueA:%d\n",slaverModule->detail.powerValueA);
     printf("powerValueB:%d\n",slaverModule->detail.powerValueB);
     printf("belongCM:%d\n",slaverModule->detail.belongCM);
     printf("belongCLP:%d\n",slaverModule->detail.belongCLP);
     printf("useFlag:%d\n",slaverModule->detail.useFlag); 

    freeModbus_TCP_Client(mb);

    return 0;
}
