#ifndef MYMODBUS_H
#define MYMODBUS_H
#include<stdio.h>
#include<stdlib.h>
#include <sys/sem.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>  
#include <fcntl.h>  
#include <string.h>  
#include <fcntl.h>  
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>  

#include <signal.h>
#include<modbus/modbus.h>

/*subdevice commnucation information*/
#define MODBUS_DEV  "/dev/ttyO4"
#define MODBUS_BUAD 115200

/* 2*2switch status */
#define  PARALLEL  0x10
#define  ACROSS    0x60 

/*Modbus RTU协议 寄存器地址划分 */
#define  OPTICALPOWER_R_ADDRESS       0x0001
#define  DISGUISE_WRITE_ADDRESS       0x000C
#define  SWITCH_POSITION_ADDRESS      0x000A
#define  ERROROUT_PORT_ADDRESS        0x000B
#define  OTDRSWITCH_W_ADDRESS         0x000F
#define  OPTICALTHRESHOLD_WR_ADDRESS  0x0011
#define  OPTICALOPTIMIZE_WR_ADDRESS   0x0021
#define  OPTICALPROTECT_WR_ADDRESS    0x0031
#define  DEVICE_BUAD_WR_ADDR          0x0041
#define  DEVICE_ID_WR_ADDR            0x0042
#define  DEVICE_MODE_WR_ADDR          0x0043
#define  DEVICE_POATCONFIG_WR_ADDR    0x0044
#define  DEVICE_STATUS                0x0054

/*Modbus TCP协议 寄存器地址划分 */

#define  MODBUS_MAX_WR_REGISTERS      0x0088
#define  NB_CONNECTION                5
#define  MBAP_LENGTH                  7
#define  NB_HIGH                      3
#define  NB_LOW                       4
#define  OFFSET_HIGH                  1
#define  OFFSET_LOW                   2

#ifndef _FC_WRITE_MULTIPLE_REGISTERS
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#endif
#ifndef _FC_READ_HOLDING_REGISTERS
#define _FC_READ_HOLDING_REGISTERS    0x03
#endif
#ifndef _FC_WRITE_SINGLE_REGISTER
#define _FC_WRITE_SINGLE_REGISTER     0x06
#endif
#define  OFF_PROTECT                  0x0000
#define  ON_onlyGROUP                 0x0001
#define  ON_onlyGATE                  0x0002
#define  ON_autoPROTECT               0x0003

#define  MODULE_NUMBER                0x0000

/*       设置从模块保护组
      SET_SLAVER_GROUP_ADDRESS+0000:模块编号(1-8)
      SET_SLAVER_GROUP_ADDRESS+0001:主端上行光路1的光路号(1->64)
      SET_SLAVER_GROUP_ADDRESS+0002:主端上行光路2的光路号(1->64)
      SET_SLAVER_GROUP_ADDRESS+0003:从端光开关位置(0x10 || 0x60)
      SET_SLAVER_GROUP_ADDRESS+0004:主从模块连接关系(0 || 1)
*/
#define  SET_SLAVER_GROUP_ADDRESS     0x0001     
#define  SET_SLAVER_GROUP_SNoA        0x0001
#define  SET_SLAVER_GROUP_SNoB        0x0002
#define  SET_SLAVER_GROUP_SwitchPos   0x0003
#define  SET_SLAVER_GROUP_Connect     0x0004


/*     设置从模块检测门限
      SET_SLAVER_GATE_ADDRESS+0x0000:模块编号(1-8)
      SET_SLAVER_GATE_ADDRESS+0x0001:主端上行光路1的检测门限(从端A1端口的检测门限)(-7000->3000)
      SET_SLAVER_GATE_ADDRESS+0x0002:主端上行光路2的检测门限(从端A1端口的检测门限)(-7000->3000)

*/
#define  SET_SLAVER_GATE_ADDRESS     0x0006
#define  SET_SLAVER_GATE_powerGateA  0x0001
#define  SET_SLAVER_GATE_powerGateB  0x0002




/*    删除从模块保护组
      DELETE_SLAVER_GROUP_ADDR+0000:模块编号
*/  
#define  DELETE_SLAVER_GROUP_ADDRESS    0x0009

/*    光开关切换请求
      DO_SLAVER_SWITCH_ADDRESS+0000:模块编号  
      DO_SLAVER_SWITCH_ADDRESS+0001:从端光开关位置 (A B同步) 
      
*/
#define DO_SLAVER_SWITCH_ADDRESS       0x000A
#define DO_SLAVER_SWITCH_Pos           0x0001



/*    模块信息 && 模块地址划分 
      MODULE_X_INFORMATION_ADDRESS+0x0000:主端上行光路号SNoA（小）
      MODULE_X_INFORMATION_ADDRESS+0x0001:主端上行光路号SNoB（大）
      MODULE_X_INFORMATION_ADDRESS+0x0002:从端光开关A的位置
      MODULE_X_INFORMATION_ADDRESS+0x0003:从端光开关B的位置
      MODULE_X_INFORMATION_ADDRESS+0x0004:从端光功率A（SNoA的）
      MODULE_X_INFORMATION_ADDRESS+0x0005:从端光功率B（SNoB的）
      MODULE_X_INFORMATION_ADDRESS+0x0006:模块所属主端CM
      MODULE_X_INFORMATION_ADDRESS+0x0007:模块所属主端CLP
      MODULE_X_INFORMATION_ADDRESS+0x0008:模块使用状况  0(无保护)   1(有保护组但无监测参数)  2(有保护组且有测试参数)
      其中   X = 1 2 3 4 5 6 7 8 
*/
#define  MODULE_1_INFORMATION_ADDRESS   0x0010
#define  MODULE_2_INFORMATION_ADDRESS   0x0020
#define  MODULE_3_INFORMATION_ADDRESS   0x0030
#define  MODULE_4_INFORMATION_ADDRESS   0x0040
#define  MODULE_5_INFORMATION_ADDRESS   0x0050
#define  MODULE_6_INFORMATION_ADDRESS   0x0060
#define  MODULE_7_INFORMATION_ADDRESS   0x0070
#define  MODULE_8_INFORMATION_ADDRESS   0x0080


#define  MODULE_INFORMATION_SNoA        0x0000
#define  MODULE_INFORMATION_SNoB        0x0001
#define  MODULE_INFORMATION_SwitchPosA  0x0002
#define  MODULE_INFORMATION_SwitchPosB  0x0003
#define  MODULE_INFORMATION_PowerValueA 0x0004
#define  MODULE_INFORMATION_PowerValueB 0x0005
#define  MODULE_INFORMATION_MasterCM    0x0006
#define  MODULE_INFORMATION_MasterCLP   0x0007
#define  MODULE_INFORMATION_Statue      0x0008

typedef struct slaverModuleInformatin
{
     struct{
	     uint16_t SNoA;
	     uint16_t SNoB;
	     uint16_t SwitchPosA;
	     uint16_t SwitchPosB;
	     uint16_t powerValueA;
	     uint16_t powerValueB;
	     uint16_t belongCM;
	     uint16_t belongCLP;
	     uint16_t useFlag;
     }detail;
     uint16_t ModNo;
}slaverModuleInformatin; 

typedef struct netInfor{
    int server_socket;

}netInfor; 

                                                                                                       
#define  MODE1_ONLINE                   1
#define  MODE2_BACKUP                   2
#define  MODE3_PROTECT_MASTER           3
#define  MODE4_ONLINE_POWER             4
#define  MODE5_PROTECT_SLAVER           5
 
union sem_modbus   
{  
    int val;  
    struct semid_ds *buf;  
    unsigned short *arry;  
}; 

modbus_t *newModbus(char *dev, int buad);   
void freeModbus(modbus_t *dev);                

modbus_t * newModBus_TCP_Server(modbus_mapping_t *mb_mapping);
void freeModbus_TCP_Server(modbus_t *ctx); 


modbus_t *newModBus_TCP_Client(char *slaverIP);
void freeModbus_TCP_Client(modbus_t *ctx); 

slaverModuleInformatin * newSlaverModule();
void freeSlaverModule(slaverModuleInformatin * me);

netInfor * newNetInfor();
void freeNetInfor(netInfor *me);

netInfor * modbus_TCP_listen(modbus_t * ctx);

int createSlaverProtectModule(modbus_t *ctx,int16_t ModNo,int16_t CM,int16_t CLP);
int setSlaverProtectGroup(modbus_t *ctx,int16_t ModNo,int16_t SNoA,int16_t SNoB,int16_t SwitchPos,int16_t ConnectPos);
int setSlaverProtectGate(modbus_t *ctx,int16_t ModNo,float powerGateA,float powerGateB);
int deleteSlaverProtectModule(modbus_t *ctx,int16_t ModNo);
int doSlaverProtectSwitch(modbus_t *ctx,int16_t ModNo,int16_t SwitchPos);
int getSlaverModuleInformation(modbus_t *ctx,slaverModuleInformatin * slaverModule);

float   getOneOpticalValue(modbus_t *mb,int SNo,int Mode);                               //光功率采集  OK
int     getMulOpticalValue(modbus_t *mb,int SNo,int16_t num,float * value);

int16_t getErrorOpticalSNo(modbus_t *mb,int exintNo);                                    //获取异常光路

int doOtdrSwitch(modbus_t * mb,int SNo,int onlyOne);                                     //执行OTDR光开关切换 OK

int setOneOpticalThreshold(modbus_t *mb,int SNo,float value);                            //设置、获取光功率阈值
int16_t setMulOpticalThreshold(modbus_t *mb,int SNo, float *value, int num);
float getOneOpticalThreshold (modbus_t *mb,int SNo);
int16_t getMulOpticalThreshold (modbus_t *mb,int SNo,float *value, int num);


int setOneOpticalOptimization(modbus_t *mb,int SNo);                                     //设置、获取光功率优化参数
int16_t setMulOpticalOptimization(modbus_t *mb,int SNo, float *value, int num);
float getOneOpticalOptimization (modbus_t *mb,int SNo);
int16_t getMulOpticalOptimization (modbus_t *mb,int SNo,float * value,int num);

int doOpticalProtectSwitch(modbus_t *mb,int SWNo,int flag,int Mode);                      //执行光保护切换
int16_t setAllProtectSwitchAcross(modbus_t *mb,int devAddr);                             //设置2*2光开关组为平行
int16_t setAllProtectSwitchParallel (modbus_t *mb,int devAddr);                          //设置2*2光开关组为交叉

int16_t setSubDeviceBuad(modbus_t *mb,int devAddr,int buad);                             //设置子单元波特率
int16_t getSubDeviceAddr(modbus_t *mb,int devAddr);                                      //匹配子单元通信地址

int16_t setSubDeviceMode(modbus_t *mb,int devAddr,int Mode);                             //设置、获取子单元模式                
int16_t getSubDeviceMode(modbus_t *mb,int devAddr);

int16_t setPortOccopy(modbus_t *mb,int devAddr,unsigned char flag);                      //设置、获取子单元光端口占用信息
unsigned char getPortOccopy(modbus_t *mb,int devAddr);                                   //获取设备状态
int16_t getSubDeviceStatus(modbus_t *mb,int devAddr);

int  initModbusPV();         //OK
int  setModbusPV();          //OK
void delModbusPV();          //OK
int  setModbus_P();          //OK
int  setModbus_V();          //OK



#endif
