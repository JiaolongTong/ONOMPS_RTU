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

#define G_MSEC_PER_SEC 1000

#define MODBUS_MAX_WR_WRITE_REGISTERS 100
static uint32_t gettime_ms(void)
{
    struct timeval tv;
#if !defined(_MSC_VER)
    gettimeofday(&tv, NULL);
    return (uint32_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else
    return GetTickCount();
#endif
}

enum {
    TCP,
    RTU
};

/* Tests based on PI-MBUS-300 documentation */
int main(int argc, char *argv[])
{
    uint8_t *tab_bit;
    uint16_t *tab_reg;
    modbus_t *ctx;
    int i;
    int nb_points;
    float elapsed;
    uint16_t wirte_data[10] = {1,2,3,4,5,6,7,8,9,10};
    struct timeval t_start,t_end;

    uint32_t rate;
    int rc;
    int n_loop;
    int use_backend;

    if (argc > 1) {
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
            n_loop = 100000;
        } else if (strcmp(argv[1], "rtu") == 0) {
            use_backend = RTU;
            n_loop = 100;
        } else {
            printf("Usage:\n  %s [tcp|rtu] - Modbus client to measure data bandwith\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        use_backend = TCP;
        n_loop =1;
    }

    if (use_backend == TCP) {
        ctx = modbus_new_tcp("192.168.0.124", 1502);
    } else {
        ctx = modbus_new_rtu("/dev/ttyUSB1", 115200, 'N', 8, 1);
        modbus_set_slave(ctx, 1);
    }
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    modbus_set_debug(ctx, TRUE);

    /* Allocate and initialize the memory to store the status */
    tab_bit = (uint8_t *) malloc(MODBUS_MAX_READ_BITS * sizeof(uint8_t));
    memset(tab_bit, 0, MODBUS_MAX_READ_BITS * sizeof(uint8_t));

    /* Allocate and initialize the memory to store the registers */
    tab_reg = (uint16_t *) malloc(MODBUS_MAX_READ_REGISTERS * sizeof(uint16_t));
    memset(tab_reg, 0, MODBUS_MAX_READ_REGISTERS * sizeof(uint16_t));

    printf("READ REGISTERS\n\n");

    nb_points = 10;

    gettimeofday(&t_start,NULL);     
    printf("Start time:%ld us\n",t_start.tv_usec);
    rc= modbus_write_registers(ctx, 0, nb_points, wirte_data);
    if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
    }
             
    int j;
    for (i=0; i<n_loop; i++) {
        rc = modbus_read_registers(ctx, 0, nb_points, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            return -1;
        }
        
        printf("Register value: ");
        for(j=0;j<rc;j++){
            printf("%x ",tab_reg[j]);
        }
        printf("\n"); 
    }
    gettimeofday(&t_end,NULL);     
    printf("End time:%ld us\n",t_end.tv_usec);

    elapsed = t_end.tv_usec - t_start.tv_usec;
    printf("Spend time:%f ms\n",elapsed/1000.0);
    /* Free the memory */
    free(tab_bit);
    free(tab_reg);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
