/********************************************************************
* 		copyright (C) 2014 all rights reserved
*			 @file: key_led.c
* 		  @Created: 2014-7-28 14:00
* 	  	   @Author: conway chen
* 	  @Description: test user keys and leds 
*	  @Modify Date: 2014-7-28 14:00
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <modbus/modbus.h>
#include "ModbusMaster.h"
/**
 * @brief: main function  
 * @Param: argc: number of parameters
 * @Param: argv: parameters list
 */
int main(int argc, char *argv[])
{
        modbus_t *mb;
        int16_t tab_reg[100]={0};
  	int tty_fd,key_fd;
	int  i = 0, count,regs;
        int mode;
	struct input_event ev_key;
	if (argc < 3) {
		fprintf(stdout, "Usage: [ UART device ] [ key device ]\n");
		exit(0);	
	}
	mb = modbus_new_rtu(argv[1],9600,'N',8,1);//open port
        key_fd=open(argv[2],O_RDWR);
	if (key_fd < 0) {
	    perror("open key device error!\n");
	    close(key_fd);
	    exit(1);
	}
        modbus_set_debug(mb, TRUE);
        modbus_set_error_recovery(mb,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);
      //  modbus_set_slave(mb,SERVER_ID);//set slave address
        if(mode=modbus_rtu_get_serial_mode(mb)<0){
            printf("get 1 serial mode faild\n");
            return -1;
        }
       mb=modbus_rtu_set_serial_mode(mb,MODBUS_RTU_RS485);
       if(mode=modbus_rtu_get_serial_mode(mb)<0){
          printf("get 2 serial mode faild\n");
          return -1;
       }
       if(mode==1)
         printf("mode:RS485 %d\n",mode);
       else
         printf("mode:RS232 %d\n",mode);
       modbus_rtu_open_485de(mb,"/sys/class/leds/RS485_TX_RX/brightness");
       modbus_connect(mb);
       for ( ; ; ) {
              count = read(key_fd, &ev_key, sizeof(struct input_event));		
              if (count > 0){
		     if (EV_KEY == ev_key.type){
                     printf("type:%d, code:%d, value:%d\n", ev_key.type, ev_key.code, ev_key.value);
                     switch(ev_key.code){
		                case 15:
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,1);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 16: {
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,2);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 17:
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,3);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 18:
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,4);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 19: 
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,5);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 20: 
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,6);//set slave address
		                        regs=modbus_read_registers(mb, 0x000B,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 21: 
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,1);//set slave address
		                        regs=modbus_read_registers(mb, 0x0001,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                      
		                     }
		                     break;
		                case 22:
		                    if (0 == ev_key.value) {
                                        modbus_set_slave(mb,1);//set slave address
		                        regs=modbus_read_registers(mb, 0x0001,1, tab_reg);
		                        for(i=0;i<regs;i++)
		                           printf("异常光路:[%d]=%d\n",i,tab_reg[i]);
		                     }
		                     break;
		               default:
		                    printf("无效中断");
		                    break;								  
		       }	
                   }
               }	
          }
        }
	close(key_fd);
        modbus_close(mb);  
        modbus_free(mb);
	return 0;
}

