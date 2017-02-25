#include<stdio.h>
#include<stdlib.h>
#include<modbus/modbus.h>
#include "ModbusMaster.h"

int main(int argc,char ** argv)
{
  modbus_t *mb;
  int mode;
  int16_t tab_reg[100]={0};
  if(argc!=4){
      printf("Uagae:%s [dev] [addr] [Buad]\n",argv[0]);
      return -1;
  }
   mb = modbus_new_rtu(argv[1],atoi(argv[3]),'N',8,1);//open port
  //mb = modbus_new_tcp("192.168.0.102", 1502);
  modbus_set_debug(mb, TRUE);

  modbus_set_error_recovery(mb,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

  modbus_set_slave(mb,atoi(argv[2]));//set slave address


  printf("Devive Addr :%d  Dirver :%s\n",atoi(argv[2]),argv[1]);
  if(mode=modbus_rtu_get_serial_mode(mb)<0)
  {
      printf("get 1 serial mode faild\n");
      return -1;
  }
  if(mode==1)
     printf("mode:RS485 %d\n",mode);
  else
     printf("mode:RS232 %d\n",mode);

  mb=modbus_rtu_set_serial_mode(mb,MODBUS_RTU_RS485);
  if(mode=modbus_rtu_get_serial_mode(mb)<0){
      printf("get 2 serial mode faild\n");
      return -1;
  }
  if(mode==1)
     printf("mode:RS485 %d\n",mode);
  else
     printf("mode:RS232 %d\n",mode);


  //modbus_rtu_open_485de(mb,"/sys/class/leds/RS485_TX_RX/brightness");


  modbus_connect(mb);



/*struct timeval old_response_timeout;
  t.tv_sec=0;
  t.tv_usec=100000000;//set modbus time 10ms
  modbus_set_response_timeout(mb,&t);
*/
/* Save original timeout */
  int function=0;
  int addr=0;
  int number=0;
  int regs;
  int i;
  int flag=1;
  modbus_read_registers(mb, 66,1, tab_reg);
  printf("地址确认:[%d]=%d\n",addr,tab_reg[0]);
while(1)
{
    printf("请选择功能:\n");
    printf("读取单个寄存器:[1] [addr] [0]\n");
    printf("读取多个寄存器:[2] [addr] [number]\n");
    printf("写入单个寄存器:[3] [addr] [value]\n");
    printf("写入多个寄存器:[4] [0] [0]\n");
    printf("退出:[0] [0] [0]\n");
    scanf("%d %d %d",&function,&addr,&number);
    printf("\n");
    switch(function){
        case 1 : 
           modbus_set_slave(mb,atoi(argv[2]));
           regs=modbus_read_registers(mb, addr,1, tab_reg);
           for(i=0;i<regs;i++)
           printf("读取单个寄存器:[%d]=%d\n",i,tab_reg[i]);
           break;
        case 2 :
           regs=modbus_read_registers(mb, addr,number, tab_reg);
           for(i=0;i<regs;i++)
           printf("读取多个寄存器:[%d]=%d\n",i,tab_reg[i]);
           break;
        case 3 :
           modbus_set_slave(mb,atoi(argv[2]));
           regs = modbus_write_register(mb,addr, number);
           printf("写入单个寄存器:Value=%d",number);
           if (regs == 1) 
              printf("成功\n");
           else
              printf("失败\n");
           break;
        case 4 :
            regs=modbus_write_registers(mb,OPTICALOPTIMIZE_WR_ADDRESS,OPTICALOPTIMIZE_WR_NUMBER,OPTICALOPTIMIZE_WR_VALUE);
            printf("写入多个寄存器:Number=%d",OPTICALOPTIMIZE_WR_NUMBER);
           if (regs == OPTICALOPTIMIZE_WR_NUMBER) 
              printf("成功\n");
           else
              printf("失败\n");  
           break;          
        default:
           goto close;
           //break;
    }
    printf("\n");
}
close:
  modbus_close(mb);  
  modbus_free(mb);
  return 0;
}
