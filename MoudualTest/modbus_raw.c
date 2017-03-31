#include <errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<modbus/modbus.h>
modbus_t    *ctx;
uint8_t     request[] = {
    0x01,
    0x14,
    0x07, /* Byte Count */
    0x06, /* SubReq1: Referecne Type - 0x06 */
    0x00, /* SubReq1: File Number - 0x0004 */
    0x04,
    0x00, /* SubReq1: Record Number - 0x0001 */
    0x01,
    0x00, /* SubReq1: Record Length - 0x0008 (16 bytes) */
    0x08,
};
uint8_t     response[100];

int main(int argc, char** argv, char** env)
{
    ctx = modbus_new_rtu(argv[1], 115200, 'N', 8, 1);
    if(!ctx)
        return -1;

    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
    //modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_NONE);
    modbus_set_debug(ctx, TRUE);
    modbus_set_slave(ctx, 1);

    if(modbus_connect(ctx) == -1) {
        modbus_free(ctx);
        return -1;
    }

    modbus_send_raw_request(ctx, request, sizeof(request));
    modbus_receive_confirmation(ctx, response);

    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
