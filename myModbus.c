#include "myModbus.h"
extern  int modbus_sem_id;  
int initModbusPV()
{
    modbus_sem_id = semget((key_t)5678, 1, 4777 | IPC_CREAT);                                //创建ModBus信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
    if(!setModbusPV())                                                                      //程序第一次被调用，初始化信号量
    {  
        fprintf(stderr, "Failed to initialize modbus_semaphore\n");  
        perror("ModBus_PV_get()");
        return -1;  
    } 
	
    return modbus_sem_id;
}

int setModbusPV()  
{  
    //用于初始化信号量，在使用信号量前必须这样做  
    union sem_modbus sem_union;  
  
    sem_union.val = 1;  
    if(semctl(modbus_sem_id, 0, SETVAL, sem_union) == -1)  
        return 0;  
    return 1;  
}  
void delModbusPV()  
{  
    //删除信号量  
    union sem_modbus sem_union;  
  
    if(semctl(modbus_sem_id, 0, IPC_RMID, sem_union) == -1)  {
        fprintf(stderr, "Failed to delete modbus_semaphore:%d\n",errno);  
        perror("ModBus_PV_del()");
    }
}  
int setModbus_P()
{
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(modbus_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_p failed\n");  
        return 0;  
    }  
    return 1;  
/*
    int ret=0;
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    while(1){ 
        if(semop(modbus_sem_id, &sem_b, 1) == -1)  
        {  
            fprintf(stderr, "modbus_semaphore_v failed:%d\n",errno);
            perror("ModBus_PV_V()");
            if(errno==EINTR) continue;
            else{
              ret=0; break; 
            }
        }else
        {
             ret=1; break; 
        } 
    }
*/
    
}
int setModbus_V()
{
    //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(modbus_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed\n");  
        return 0;  
    }  
    return 1;  
/*
    int ret=0;  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO; 
    while(1){ 
        if(semop(modbus_sem_id, &sem_b, 1) == -1)  
        {  
            fprintf(stderr, "modbus_semaphore_v failed:%d\n",errno);
            perror("ModBus_PV_V()");
            if(errno==EINTR) continue;
            else{
              ret=0; break; 
            }
        }else
        {
             ret=1; break; 
        } 
    }
    return ret;  
*/
}

modbus_t *newModbus(char *dev, int buad)
{
  modbus_t *mb;
  int mode;
  mb = modbus_new_rtu(dev,buad,'N',8,1);//open port
  modbus_set_debug(mb, FALSE);

  modbus_set_error_recovery(mb,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

  if(mode=modbus_rtu_get_serial_mode(mb)<0)
  {
      printf("get 1 serial mode faild\n");
      return (modbus_t *)-1;
  }

  mb=modbus_rtu_set_serial_mode(mb,MODBUS_RTU_RS485);
  if(mode=modbus_rtu_get_serial_mode(mb)<0){
      printf("get 2 serial mode faild\n");
      return (modbus_t *)-1;
  }
  modbus_rtu_open_485de(mb,"/sys/class/leds/RS485_TX_RX/brightness");
  modbus_connect(mb);
  
  return mb;
}
void freeModbus(modbus_t *mb)
{
  modbus_close(mb);  
  modbus_free(mb);
}

float getOneOpticalValue(modbus_t *mb,int SNo)                                       //光功率采集
{
       int16_t tab_reg[100]={0};
       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SNo-1)/8)+1;
       subPort =((SNo-1)%8);
     //  if(mb->debug){
     //    printf("SNo = %d devAddr=%d,subPort=%d\n",SNo,devAddr,OPTICALPOWER_R_ADDRESS+subPort);
     //  }
       modbus_set_slave(mb,devAddr);       
       regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+subPort,1, tab_reg);
     //  if(mb->debug){
     //     for(i=0;i<regs;i++)
     //        printf("SNo:%d   PowerValue:%f\n",OPTICALPOWER_R_ADDRESS+subPort,((float)tab_reg[i])/100.0);
     //  }
      return ((float)tab_reg[0])/100.0;

}
int getMulOpticalValue(modbus_t *mb,int SNo,int16_t num,float * value)
{
       int16_t tab_reg[100]={0};
       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SNo-1)/8)+1;
       subPort =((SNo-1)%8);
       modbus_set_slave(mb,devAddr);       
       regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+subPort,num, tab_reg);

          for(i=0;i<regs;i++){
           //  if(mb->debug){
           //      printf("光路号:%d   光功率值:%d=%f\n",subPort,((float)tab_reg[i])/100.0); 
           //  }
             value[i] =  ((float)tab_reg[i])/100.0;
          }

      return regs;
}


int doOtdrSwitch(modbus_t * mb,int SNo)
{
       int regs;
       int PortA,PortB;
       PortA =((SNo-1)/8)+1;                    //OTDR子单元的1*8光开关号
       PortB =((SNo-1)%8)+1;                      //光功率模块上的1*8光开关号
       modbus_set_slave(mb,0);              //OTDR子单元地址 
       regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS,PortA);
       
       if(1 == regs){
          //if(mb->debug){
            printf("第一组1*8光开关切换成功-->PortA=%d\n",PortA);
          //}
       }else
       {
          //if(mb->debug){
             printf("第一组1*8光开关切换失败->back=%d\n",regs);
          // }
           return -1;
       }

       modbus_set_slave(mb,PortA);          //光功率模块子单元地址   1-8
       usleep(100000);
       regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS,PortB);
       if(1 == regs){
           //if(mb->debug){
              printf("第二组1*8光开关切换成功-->PortB=%d\n",PortB);
           //}
       }else
       {
           //if(mb->debug){
              printf("第二组1*8光开关切换失败-->back=%d\n",regs);
           //}
           return -1;
       }

      return 0;
}


int setOneOpticalThreshold(modbus_t *mb,int SNo,float value)
{

       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SNo-1)/8)+1;
       subPort =((SNo-1)%8);
       modbus_set_slave(mb,devAddr);       
       regs = modbus_write_register(mb,OPTICALTHRESHOLD_WR_ADDRESS+subPort, (int)(value*100));
       if(1 == regs){
          //if(mb->debug){
             printf("设置光功率阈值成功-->子单元号=%d 光路号=0X%X 阈值=%d \n",devAddr,OPTICALTHRESHOLD_WR_ADDRESS+subPort,(int)(value*100));
          //}
       }else
       {
          //if(mb->debug){
             printf("设置光功率阈值失败-->back=%d\n",regs);
          //}
           return -1;
       }
    
      return 0;
}

float getOneOpticalThreshold(modbus_t *mb,int SNo)
{
       int16_t value[100]={0};
       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SNo-1)/8)+1;
       subPort =((SNo-1)%8);
       modbus_set_slave(mb,devAddr);       
        regs=modbus_read_registers(mb, OPTICALTHRESHOLD_WR_ADDRESS+subPort,1, value);
       if(1 == regs){
          //if(mb->debug){
          //   printf("获取光功率阈值成功-->子单元号=%d 光路号=%d 阈值=%f \n",devAddr,subPort+1,((float)value[0])/100.0);
          //}
       }else
       {
          //if(mb->debug){
          //   printf("获取光功率阈值失败-->back=%d\n",regs);
          //}
           return (float)-1;
       }
    
      return ((float)value[0])/100.0;
}

int doOpticalProtectSwitch(modbus_t *mb,int PNo,int flag)
{

       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((PNo-1)/4)+1;
       subPort =((PNo-1)%4);
       modbus_set_slave(mb,devAddr);       
       regs = modbus_write_register(mb,OPTICALPROTECT_WR_ADDRESS+subPort,flag);
       if(1 == regs){
          //if(mb->debug){
            printf("执行光保护切换成功-->子单元号=%d 保护组号=%d 执行状态={%s} \n",devAddr,subPort+1,flag==PARALLEL?"平行":"交叉");
          //}
       }else
       {

         //if(mb->debug){
           printf("执行光保护切换失败-->子单元号=%d 保护组号=%d 执行状态={%s} \n",devAddr,subPort+1,flag==PARALLEL?"平行":"交叉");
         //}

           return -1;
       }
    
      return 0;


}

