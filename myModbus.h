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
#define MODBUS_BUAD 9600

/* 2*2switch status */
#define  PARALLEL  0x10
#define  ACROSS    0x60

/*Modbus registers map*/
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


union sem_modbus   
{  
    int val;  
    struct semid_ds *buf;  
    unsigned short *arry;  
}; 

modbus_t *newModbus(char *dev, int buad);      //OK
void freeModbus(modbus_t *dev);                //OK

float   getOneOpticalValue(modbus_t *mb,int SNo);                                       //光功率采集  OK
int     getMulOpticalValue(modbus_t *mb,int SNo,int16_t num,float * value);


//int16_t getOtdrSwitchPositon(modbus_t *mb);

int16_t getErrorOpticalSNo(modbus_t *mb,int exintNo);                                    //获取异常光路

int doOtdrSwitch(modbus_t * mb,int SNo);                                                 //执行OTDR光开关切换 OK

int setOneOpticalThreshold(modbus_t *mb,int SNo,float value);                        //设置、获取光功率阈值
int16_t setMulOpticalThreshold(modbus_t *mb,int SNo, float *value, int num);
float getOneOpticalThreshold (modbus_t *mb,int SNo);
int16_t getMulOpticalThreshold (modbus_t *mb,int SNo,float * value,int num);


int setOneOpticalOptimization(modbus_t *mb,int SNo);                                 //设置、获取光功率优化参数
int16_t setMulOpticalOptimization(modbus_t *mb,int SNo, float *value, int num);
float getOneOpticalOptimization (modbus_t *mb,int SNo);
int16_t getMulOpticalOptimization (modbus_t *mb,int SNo,float * value,int num);

int doOpticalProtectSwitch(modbus_t *mb,int PNo,int flag);                               //执行光保护切换
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
