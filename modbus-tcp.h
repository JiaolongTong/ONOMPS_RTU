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

#define G_MSEC_PER_SEC 1000

#define MODBUS_MAX_WR_WRITE_REGISTERS 100

modbus_t *newTCPMaster(char *slaveIP, int buad);
modbus_t *newTCPSlave(char *dev,int buad); 


#endif
