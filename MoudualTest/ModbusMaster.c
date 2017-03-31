#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>  
#include <modbus/modbus.h>
#include "ModbusMaster.h"

enum {
    TCP,
    TCP_PI,
    RTU
};

int main(int argc, char *argv[])
{
    uint16_t *read_data;              //读取数据缓冲区
    int16_t  *read_data_float;
    modbus_t *ctx;
    float value;
    int i;
    int nb_points;
    int rc;
    float real;
    uint32_t ireal;
    struct timeval old_response_timeout;
    struct timeval response_timeout;
    int use_backend;
    use_backend = RTU;
    ctx = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    modbus_set_debug(ctx, TRUE);
/*
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);
*/
    if (use_backend == RTU) {
          modbus_set_slave(ctx, SERVER_ID);
    }

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Allocate and initialize the memory to store the registers */
    nb_points = MAX_REGISTERS;
    read_data = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
    memset(read_data, 0, nb_points * sizeof(uint16_t));
    nb_points = MAX_REGISTERS;
    read_data_float = (int16_t *) malloc(nb_points * sizeof(int16_t));
    memset(read_data_float, 0, nb_points * sizeof(int16_t));
    printf("** UNIT TESTING **\n");
    /* read switch position register */
    rc = modbus_write_register(ctx,SWITCH_OTDR_ADDRESS, SWITCH_OTDR_VALUE);
    printf("1/1 执行OTDR光开关切换:SNo=%d",SWITCH_OTDR_VALUE);
    if (rc == 1) {
        printf(" OK\n");
    } else {
        printf("FAILED\n");
        goto close;
    }

/*
    rc = modbus_read_registers(ctx, SWITCH_POSITION_ADDRESS,
                               1, read_data);
    printf("2/1 获取当前OTDR光开关位置:SNo=%d",read_data[0]);
    if (rc != 1) {
        printf("FAILED (nb points %d)\n", rc);
        goto close;
    }

    if (read_data[0] != 0x0004) {
        printf("FAILED (%0X != %0X)\n",
               read_data[0], 0x0004);
        goto close;
    }
    printf(" OK\n");


    /* End of single register */

    /* Many registers 
    rc = modbus_write_registers(ctx, OPTICALTHRESHOLD_WR_ADDRESS,
                                OPTICALTHRESHOLD_WR_NUMBER, OPTICALTHRESHOLD_WR_VALUE);
    printf("1/2 写入多路光功率阈值: ");
    if (rc == OPTICALTHRESHOLD_WR_NUMBER) {
        printf(" OK\n");
    } else {
        printf("FAILED\n");
        goto close;
    }

    rc = modbus_read_registers(ctx, OPTICALTHRESHOLD_WR_ADDRESS,
                               OPTICALTHRESHOLD_WR_NUMBER, read_data_float);
    printf("2/2 读取多路光功率阈值:\n");
    if (rc != OPTICALTHRESHOLD_WR_NUMBER) {
        printf("FAILED (nb points %d)\n", rc);
        goto close;
    }
   


    for (i=0; i < OPTICALTHRESHOLD_WR_NUMBER; i++) {
        if (read_data_float[i] == OPTICALTHRESHOLD_WR_VALUE[i]) {
            value=((float)read_data_float[i])/100.00;
            printf("光路:%d 光功率阈值:%f\n",
                   i+1,
                   value);
        }else
        {
            goto close;
        }
    }
    printf(" OK\n");





    rc = modbus_write_register(ctx, DISGUISE_WRITE_ADDRESS,
                               DISGUISE_WRITE_VALUE);
    printf("1/3 写入多路光功率实时值:(伪写 )");
    if (rc == 1) {
        printf(" OK\n");
    } else {
        printf("FAILED\n");
        goto close;
    }

 
    rc = modbus_read_registers(ctx, OPTICALPOWER_R_ADDRESS,
                               OPTICALPOWER_R_NUMBER, read_data_float);
    printf("1/3 读取多路光功率实时数据:\n");

    if (rc != OPTICALPOWER_R_NUMBER) {
        printf("FAILED (nb points %d)\n", rc);
        goto close;
    }else
    {
     for (i=0; i < OPTICALPOWER_R_NUMBER; i++) {
            value=((float)read_data_float[i])/100.00;
            printf("光路:%d 光功率实时数据:%f\n",
            i+1,
            value);
        }
    }
    printf(" OK\n");
*/

    printf("\n所有测试成功\n");

close:
    /* Free the memory */
    free(read_data);
    free(read_data_float);
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);
    printf("\n结束\n");
    return 0;
}
