
#include "myModbus.h"
extern   int modbus_sem_id;  
#define  Delag_TIME  0     //us
const int16_t  OpticalPower_PATH_Mode2_Mode4[] = { 0x0008, 0x0007, 0x0006, 0x0005, 0x0004,0x0003, 0x0002, 0x0001};       //光功率采集通道选择，备纤模式+在纤（OPM）模式   (脚标是光路号1-8)
const int16_t  OpticalPower_PATH_Mode3[]       = { 0x0008, 0x0000, 0x0006, 0x0000, 0x0007,0x0000, 0x0005, 0x0000};       //光功率采集通道选择,保护模式 (脚标是光路号1-8)
const int16_t  OpticalPower_PATH_Mode5[]       = { 0x0008, 0x0007};
const int16_t  OpticalProtect_PATH_Mode3[]     = { 0x0003, 0x0002, 0x0001, 0x0004};                                      //光保护通道选择1*2光开关,保护模式
const int16_t  OpticalProtect_PATH_Mode5[]     = { 0x0003, 0x0004};                                                      //光保护通道选择1*2光开关,保护模式
const int16_t  OpticalOTDR_PATH_Ordinary[]     = { 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,0x0006, 0x0007, 0x0008};       //一般模式OTDR通道选择1*8光开关
const int16_t  OpticalOTDR_PATH_Protect[]      = { 0x0001, 0x0004, 0x0005, 0x0008, 0x0002,0x0003, 0x0006, 0x0007};       //保护模式OTDR通道选择1*8光开关

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
}


/*
               ModBus-RTU API

*/
modbus_t *newModbus(char *dev, int buad)
{
  modbus_t *mb;
  int mode;
  //usleep(Delag_TIME);
  mb = modbus_new_rtu(dev,buad,'N',8,1);
  modbus_set_debug(mb, FALSE);

  modbus_set_error_recovery(mb,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

  modbus_connect(mb);
  
  return mb;
}

void freeModbus(modbus_t *mb)
{
  modbus_close(mb);  
  modbus_free(mb);
}

float   getOneOpticalValue(modbus_t *mb,int SNo,int Mode)   //光功率采集                                      
{
       int16_t tab_reg[100]={0};
       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SNo-1)/8)+1;
       subPort =((SNo-1)%8);
       //usleep(Delag_TIME);
       modbus_set_slave(mb,devAddr);   
       switch(Mode){
          case  MODE2_BACKUP          :
          case  MODE4_ONLINE_POWER    :regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+OpticalPower_PATH_Mode2_Mode4[subPort]-1,1,tab_reg);break;
          case  MODE3_PROTECT_MASTER  :regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+OpticalPower_PATH_Mode3[subPort]-1,1, tab_reg);break;
          case  MODE5_PROTECT_SLAVER  :regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+OpticalPower_PATH_Mode5[subPort]-1,1, tab_reg); break; 
          default  : return regs;
       }
                   
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
       //usleep(Delag_TIME);
       modbus_set_slave(mb,devAddr);       
       regs=modbus_read_registers(mb,OPTICALPOWER_R_ADDRESS+subPort,num, tab_reg);
       for(i=0;i<regs;i++){
             value[i] =  ((float)tab_reg[i])/100.0;
       }
      return regs;
}


int doOtdrSwitch(modbus_t * mb,int SNo,int onlyOne,int ModType)
{
       int regs=-1;
       int otdrPort,subPort;
       otdrPort =((SNo-1)/8)+1;                    //OTDR子单元的1*8光开关号
       subPort =((SNo-1)%8)+1;                     //功能子单元模块上的1*8光开关号
       //usleep(Delag_TIME);
       modbus_set_slave(mb,0);                     //OTDR单元地址 
       if(!onlyOne){ 
               if(ModType == MODE3_PROTECT_MASTER )
                  regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS,otdrPort);
               else
                  regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS,otdrPort);

	       if(1 == regs){
		     printf("OTDR单元模块1*8光开关切换成功-->otdrPort=%d\n",otdrPort);
	       }else
	       {
		     printf("OTDR单元模块1*8光开关切换失败->back=%d\n",regs);
		   return -1;
	       }
       }
       modbus_set_slave(mb,otdrPort);              //功能子单元地址   1-8
       //usleep(Delag_TIME);
       if( ModType ==  MODE3_PROTECT_MASTER )
             regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS, OpticalOTDR_PATH_Protect[subPort-1]);
       else
             regs=modbus_write_register(mb,OTDRSWITCH_W_ADDRESS,OpticalOTDR_PATH_Ordinary[subPort-1]);
       if(1 == regs){
              printf("功能子单元模块1*8光开关切换成功-->subPort=%d\n",subPort);
       }else
       {
              printf("功能子单元模块1*8光开关切换失败-->back=%d\n",regs);
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
       //usleep(Delag_TIME);
       modbus_set_slave(mb,devAddr);       
       regs = modbus_write_register(mb,OPTICALTHRESHOLD_WR_ADDRESS+subPort, (int)(value*100));
       if(1 == regs){
          //if(mb->debug){
             printf("设置光功率阈值成功-->子单元号=%d 光路号=0X%X 阈值=%d \n",devAddr,OPTICALTHRESHOLD_WR_ADDRESS+subPort,(int)(value*100));
          //}s
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
       //usleep(Delag_TIME); 
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


int16_t setSubDeviceMode(modbus_t *mb,int devAddr,int Mode)
{

     uint16_t  value[100]={0};
     int reg;
     //usleep(Delag_TIME); 
     modbus_set_slave(mb,devAddr);   // 1-8
     reg=modbus_read_registers(mb, DEVICE_ID_WR_ADDR,1, value);
     if(value[0] == devAddr && reg != -1){
           usleep(100000);
           reg = modbus_write_register(mb,DEVICE_MODE_WR_ADDR,Mode);
           if(reg != -1){
                return 0;
           }else{
                printf("Write Device Mode Failed!\n");
                return -1;
           }
     }else{
           printf("Don't have Device,Connect Failed!\n");
           return -1;
     }
     
}
int doOpticalProtectSwitch(modbus_t *mb,int SWNo,int flag,int Mode)
{

       int regs,i;
       int16_t devAddr;
       int16_t subPort;
       devAddr =((SWNo-1)/4)+1;
       subPort =((SWNo-1)%4);
       //usleep(Delag_TIME); 
       modbus_set_slave(mb,devAddr);       
       switch(Mode){
           case MODE3_PROTECT_MASTER : regs = modbus_write_register(mb,OPTICALPROTECT_WR_ADDRESS+OpticalProtect_PATH_Mode3[subPort]-1,flag);break;
           case MODE5_PROTECT_SLAVER : regs = modbus_write_register(mb,OPTICALPROTECT_WR_ADDRESS+OpticalProtect_PATH_Mode5[subPort]-1,flag);break;
           default: return -1;
       }
       
       if(1 == regs){
          //if(mb->debug){
            printf("执行光保护切换成功-->子单元号=%d 保护组号=%d 执行状态={%s} \n",devAddr,subPort+1,flag==PARALLEL?"平行":"交叉");
         // }
       }else
       {

         //if(mb->debug){
           printf("执行光保护切换失败-->子单元号=%d 保护组号=%d 执行状态={%s} \n",devAddr,subPort+1,flag==PARALLEL?"平行":"交叉");
         //}
         return -1;
       }
    
      return regs;
}



/*
               ModBus-TCP API

*/


slaverModuleInformatin * newSlaverModule()
{
      slaverModuleInformatin * me = (slaverModuleInformatin *) malloc(sizeof(slaverModuleInformatin));
      return me;
}

void freeSlaverModule(slaverModuleInformatin * me)
{
     free(me);
} 

netInfor * newNetInfor()
{
      netInfor *  me = (netInfor *) malloc(sizeof(netInfor));
      return me;
}

void freeNetInfor(netInfor *me){
     free(me);
}
modbus_t *newModBus_TCP_Client(char *slaverIP)
{
    int16_t *tab_reg;
    modbus_t *ctx;
    int rc;
    ctx = modbus_new_tcp(slaverIP, 1502);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }
    modbus_set_debug(ctx, FALSE);
    return ctx;

}
void freeModbus_TCP_Client(modbus_t *ctx)
{
  modbus_close(ctx);  
  modbus_free(ctx);
}

modbus_t * newModBus_TCP_Server(modbus_mapping_t *mb_mapping)
{
    modbus_t *ctx =NULL;
    ctx = modbus_new_tcp("0.0.0.0", 1502);

    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);

    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }
    modbus_set_debug(ctx, FALSE);
    return ctx; 
}

netInfor * modbus_TCP_listen(modbus_t * ctx)
{

    netInfor * netinfor=NULL;
    netinfor=newNetInfor();
    int server_socket;
 
    netinfor->server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);

    if ( netinfor->server_socket == -1) {
        fprintf(stderr, "Unable to listen TCP connection\n");
        modbus_free(ctx);
        return (netInfor *)-1;
    }

    return netinfor;
}




int createSlaverProtectModule(modbus_t *ctx,int16_t ModNo,int16_t CM,int16_t CLP)
{
     int16_t addr=0;
     int     rc;
     int16_t write_date[3];
     write_date[0] = CM;
     write_date[1] = CLP;
     write_date[2] = 0;
     addr=ModNo*0x0010;

     rc = modbus_write_registers(ctx,addr+MODULE_INFORMATION_MasterCM,3,write_date);   
     if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
     }
     return rc;
}


int setSlaverProtectGroup(modbus_t *ctx,int16_t ModNo,int16_t SNoA,int16_t SNoB,int16_t SwitchPos,int16_t ConnectPos )
{
     int16_t rc;
     int16_t write_date[5];
     write_date[0] = ModNo;
     write_date[1] = SNoA;
     write_date[2] = SNoB;
     write_date[3] = SwitchPos;
     write_date[4] = ConnectPos;

     rc = modbus_write_registers(ctx,SET_SLAVER_GROUP_ADDRESS,5,write_date);   
     if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
     }
     return rc;
}

int setSlaverProtectGate(modbus_t *ctx,int16_t ModNo,float powerGateA,float powerGateB)
{
     int16_t rc;
     int16_t write_date[3];
     write_date[0] = ModNo;
     write_date[1] = (int16_t)(powerGateA*100);
     write_date[2] = (int16_t)(powerGateB*100);

      printf("%d  %d \n",write_date[1],write_date[2]);
     rc = modbus_write_registers(ctx,SET_SLAVER_GATE_ADDRESS,3,write_date);   
     if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
     }
     return rc;
}

int deleteSlaverProtectModule(modbus_t *ctx,int16_t ModNo)
{

     int16_t rc;
     int16_t write_date[1];
     write_date[0] = ModNo;
     rc = modbus_write_registers(ctx,DELETE_SLAVER_GROUP_ADDRESS,1,write_date);   
     if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
     }
     return rc;
}

int doSlaverProtectSwitch(modbus_t *ctx,int16_t ModNo,int16_t SwitchPos){

     int16_t rc;
     int16_t write_date[2];
     write_date[0] = ModNo;
     write_date[1] = SwitchPos;
     
     rc = modbus_write_registers(ctx,DO_SLAVER_SWITCH_ADDRESS,2,write_date);   
     if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
     }
     return rc;


}
int getSlaverModuleInformation(modbus_t *ctx,slaverModuleInformatin * slaverModule)
{

     uint16_t addr=0,regs=0;
     uint16_t *read_date =NULL;
     read_date = (uint16_t *)malloc(sizeof(uint16_t)*10);
     addr= slaverModule->ModNo*16;
     read_date[9] = slaverModule->ModNo;
     if(regs=modbus_read_registers(ctx,addr,9,read_date )<0){

        fprintf(stderr, "%s\n", modbus_strerror(errno));  
        return -1;        
     }
     memcpy(slaverModule,read_date, sizeof(slaverModuleInformatin));
     free(read_date);
     read_date=NULL;
     return regs;
}

int initSlaver_Full(modbus_t *ctx,int16_t ModNo,int16_t CM,int16_t CLP,float powerGateA,float powerGateB,int16_t SNoA,int16_t SNoB,int16_t SwitchPos,int16_t ConnectPos){

        int flagMasterA,flagMasterB,flagMasterC;
	if(flagMasterA=createSlaverProtectModule(ctx,ModNo,CM,CLP)<0){
		printf("Error to create new slaver protect module!\n");   
	}else{
		printf("Create slaver protect module successful!\n");
	}
	if(flagMasterB=setSlaverProtectGroup(ctx,ModNo,SNoA,SNoB,SwitchPos,ConnectPos)<0){
		printf("Error to set slaver protect group!\n");   
	}else{
		printf("set slaver protect group successful!\n");
	}
	if(flagMasterC=setSlaverProtectGate(ctx,ModNo,powerGateA,powerGateB)<0){
		printf("Error to set slaver protect gate!\n"); 
	}else{
		printf(" set slaver protect gate successful!\n");
	}
        if(flagMasterA>=0 && flagMasterB>=0 && flagMasterC>=0)return 0;
        else return -1;
}

int initSlaver_ModuelAndGroup(modbus_t *ctx,int16_t ModNo,int16_t CM,int16_t CLP,int16_t SNoA,int16_t SNoB,int16_t SwitchPos,int16_t ConnectPos){
        int flagMasterA,flagMasterB;
	if(flagMasterA=createSlaverProtectModule(ctx,ModNo,CM,CLP)<0){
		printf("Error to create new slaver protect module!\n");   
	}else{
		printf("Create slaver protect module successful!\n");
	}
	if(flagMasterB=setSlaverProtectGroup(ctx,ModNo,SNoA,SNoB,SwitchPos,ConnectPos)<0){
		printf("Error to set slaver protect group!\n");   
	}else{
		printf("set slaver protect group successful!\n");
	}
        if(flagMasterA>=0 && flagMasterB>=0)return 0;
        else return -1;
}









