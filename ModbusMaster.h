/* tests/unit-test.h.  Generated from unit-test.h.in by configure.  */
/*
 * Copyright © 2008-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

/* Constants defined by configure.ac */
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# ifndef _MSC_VER
# include <stdint.h>
# else
# include "stdint.h"
# endif
#endif

#define SERVER_ID         0
#define INVALID_SERVER_ID 32

const uint16_t MAX_REGISTERS =100;       //0x0064

/*optical power registers*/

const uint16_t OPTICALPOWER_R_ADDRESS = 0x0001;
const uint16_t OPTICALPOWER_R_NUMBER  = 8;
const int16_t  OPTICALPOWER_R_VALUE[] = { 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
                                          0x0006, 0x0007, 0x0008, 0x0009, 0x000A};
const uint16_t DISGUISE_WRITE_ADDRESS = 0x000C;
const uint16_t DISGUISE_WRITE_VALUE   = 0x0000;
/*optical position for OTDR test*/
const uint16_t SWITCH_POSITION_ADDRESS =0x000A;
const uint16_t SWITCH_POSITION_VALUE =0x0001; /*1-8*/
const uint16_t SWITCH_OTDR_ADDRESS =0x000F;
const uint16_t SWITCH_OTDR_VALUE   =0x0003;



/* optical threshold registers */
const uint16_t OPTICALTHRESHOLD_WR_ADDRESS = 0x0011;
const uint16_t OPTICALTHRESHOLD_WR_NUMBER  = 8;
const int16_t OPTICALTHRESHOLD_WR_VALUE[] = { 1211, -1212, 1213, -1214, 1215,
                                              1216, -1217, 1218, -1219, 1301};


/* optical power optimize registers */
const uint16_t OPTICALOPTIMIZE_WR_ADDRESS = 0x0021;
const uint16_t OPTICALOPTIMIZE_WR_NUMBER  = 8;
const uint16_t OPTICALOPTIMIZE_WR_VALUE[] = { 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
                                          0x0006, 0x0007, 0x0008, 0x0009, 0x000A};

/*  optical protect registers */
const uint16_t OPTICALPROTECT_WR_ADDRESS = 0x0031;
const uint16_t OPTICALPROTECT_WR_NUMBER  = 8;
const uint16_t OPTICALPROTECT_WR_VALUE[] = { 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
                                          0x0006, 0x0007, 0x0008, 0x0009, 0x000A};
/*get or set device baudrate*/
const uint16_t DEVICE_RD_BUAD_ADDR = 0x0041;
const uint16_t DEVICE_RD_BUAD      = 0x0000;

/*get or set device SERVER_ID*/
const uint16_t DEVICE_ID_WR_ADDR = 0x0042;
const uint16_t DEVICE_ID         = 0x0000;


/*device Serial Number*/
const uint16_t DEVICE_SN_WR_ADDR = 0x0051;
const unsigned char DEVICE_SN[]  = {0x20,0x10,0x06,0x17};

/*device production date */
const uint16_t DEVICE_MFGDATE_WR_ADDR = 0x0052;
const unsigned char DEVICE_MFGDATE[]  = {0x20,0x10,0x06,0x17};

/*device Software version*/
const uint16_t DEVICE_VERSION_WR_ADDR = 0x0053;
const unsigned char DEVICE_VERSION[]  = {0x00,0x00,0x00,0x10};

/*device Status*/
const uint16_t DEVICE_STATUS_R_ADDR   = 0x0054;
const unsigned char DEVICE_STATUS[]   = {0x00,0x00,0x00,0x10};

#endif /* _UNIT_TEST_H_ */
