/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <modbus/modbus.h>

#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "myModbus.h"
#include "common.h"

int    modbus_sem_id;  
static modbus_mapping_t *modbus_mapping;
static modbus_t *ctx = NULL;
static netInfor serverInfor;

static void close_sigint(int dummy)
{
    if (serverInfor.server_socket != -1) {
	close(serverInfor.server_socket);
    }
    modbus_free(ctx);
    modbus_mapping_free(modbus_mapping);

    printf("exit normally!\n");
    exit(dummy);
}

int main(void)
{
    int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket,server_socket;
    fd_set rdset;
    fd_set refset;
    int    fdmax,rc,j;
    int    RegisterNumber,WriteAddress,ReadAddress;
    slaverModuleInformatin  *moduleState=NULL;

    ctx = modbus_new_tcp("0.0.0.0", 1502);
    modbus_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (modbus_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    serverInfor.server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);
    if (serverInfor.server_socket == -1) {
        fprintf(stderr, "Unable to listen TCP connection\n");
        modbus_free(ctx);
        return -1;
    }
    signal(SIGINT, close_sigint);


    FD_ZERO(&refset);

    FD_SET(serverInfor.server_socket, &refset);

  
    fdmax = serverInfor.server_socket;


    for(;;){

        rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) {
            perror("Server select() failure.");
            close_sigint(1);
        }

        for (master_socket = 0; master_socket <= fdmax; master_socket++) {

            if (!FD_ISSET(master_socket, &rdset)) {
                continue;
            }

            if (master_socket ==serverInfor.server_socket) {
                
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(serverInfor.server_socket, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    perror("Server accept() error");
                } else {
                    FD_SET(newfd, &refset);

                    if (newfd > fdmax) {

                        fdmax = newfd;
                    }
                    printf("New connection from %s:%d on socket %d\n",
                           inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            } else {

                modbus_set_socket(ctx, master_socket);
                
                rc = modbus_receive(ctx, query);

                if (rc > 0) {

                    modbus_reply(ctx, query, rc, modbus_mapping);
             
                    if(query[MBAP_LENGTH] == _FC_WRITE_MULTIPLE_REGISTERS){    //0x10
                       RegisterNumber    = (query[MBAP_LENGTH + NB_HIGH] << 8)     + query[MBAP_LENGTH + NB_LOW];
                       WriteAddress      = (query[MBAP_LENGTH + OFFSET_HIGH] << 8) + query[MBAP_LENGTH + OFFSET_LOW];                                                                                       
                       switch(WriteAddress){
                            case SET_SLAVER_GROUP_ADDRESS:{
                                     printf("设置从模块保护组!\n",WriteAddress); 
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;                                  
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);  
                                     moduleState->detail.SNoA       = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SNoA];
                                     moduleState->detail.SNoB       = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SNoB];  
                                     moduleState->detail.SwitchPosA = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SwitchPos];
                                     moduleState->detail.SwitchPosB = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SwitchPos];
                                     moduleState->detail.useFlag   |= ON_onlyGROUP; 
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     //更新数据库
                                     //插入测试节点
                                     if(moduleState->detail.useFlag==ON_autoPROTECT){
                                           printf("开启自动保护测试,模块%d!\n",moduleState->ModNo);
                                           //linkHead_check_A=insertNode_from_modbusTCP(moduleState->ModNo);
                                     }                                    
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                 }break;
                            case SET_SLAVER_GATE_ADDRESS:{
                                     printf("设置从模块检测门限   mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;                                  
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);  
                                     moduleState->detail.useFlag    |= ON_onlyGATE;
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     //更新数据库
                                     //插入测试节点
                                     if(moduleState->detail.useFlag==ON_autoPROTECT){
                                           printf("开启自动保护测试,模块%d!\n",moduleState->ModNo); 
                                           //linkHead_check_A=insertNode_from_modbusTCP(moduleState->ModNo);
                                     }
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                 }break;
                            case DELETE_SLAVER_GROUP_ADDRESS:{
                                     printf("删除从模块保护组     mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;  
                                     moduleState->detail.SNoA       = 0;
                                     moduleState->detail.SNoB       = 0;  
                                     moduleState->detail.SwitchPosA = 0;
                                     moduleState->detail.SwitchPosB = 0;
                                     moduleState->detail.powerValueA= 0;
                                     moduleState->detail.powerValueB= 0;  
                                     moduleState->detail.belongCM   = 0;
                                     moduleState->detail.belongCLP  = 0;
                                     moduleState->detail.useFlag    = 0;
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     printf("结束自动保护测试,模块%d!\n",moduleState->ModNo); 
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                 }break;
                            case DO_SLAVER_SWITCH_ADDRESS:{
                                     printf("手动切换光开关       mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;                                  
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);
                                     if(moduleState->detail.useFlag==ON_autoPROTECT || moduleState->detail.useFlag==ON_onlyGROUP){   
                                     	moduleState->detail.SwitchPosA = modbus_mapping->tab_registers[WriteAddress+DO_SLAVER_SWITCH_Pos];
                                     	moduleState->detail.SwitchPosB = modbus_mapping->tab_registers[WriteAddress+DO_SLAVER_SWITCH_Pos];
                                     	memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);   
                                     }    
                                     freeSlaverModule(moduleState);
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                 }break;
                            case MODULE_1_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                
                            case MODULE_2_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                 
                            case MODULE_3_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                 
                            case MODULE_4_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_5_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_6_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_7_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_8_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:{
                                     printf("创建保护从模块_%x    mb_mapping[%d]!\n",WriteAddress/0x0010,WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=WriteAddress/0x0010;
                                     ReadAddress = moduleState->ModNo*0x0010;
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);
                                     moduleState->detail.useFlag    = OFF_PROTECT;
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
     
                                 }break;
                            default: printf("该地址不支持写操作   mb_mapping[%d]!\n",WriteAddress);
                       }
 
                    }else if(query[MBAP_LENGTH] == _FC_READ_HOLDING_REGISTERS){//0x03
                    }else if(query[MBAP_LENGTH] ==  _FC_WRITE_SINGLE_REGISTER){//0x06
                    }else{
                         printf("Don't support funtion!\n");
                    }
                } else if (rc == -1) {

                    printf("Connection closed on socket %d\n", master_socket);
                    close(master_socket);

                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
                }
            }
        }
    }

    return 0;
}
