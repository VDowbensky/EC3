//-----------------------------------------------------------------------------
// C2_Utils.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for low-level C2 interface routines
// used by C2_Flash.c.
//
//-----------------------------------------------------------------------------

#ifndef C2_UTILS_H
#define C2_UTILS_H

#include <compiler_defs.h>
#include "Devices.h"

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern U8 C2_Reset (void);
extern U8 C2_WriteAR (U8 addr);
extern U8 C2_ReadAR (void);
extern U8 C2_WriteDR (U8 dat, U16 timeout_us);
extern U8 C2_ReadDR (U16 timeout_us);

extern U8 C2_Poll_InBusy (U16 timeout_ms);
extern U8 C2_Poll_OutReady (U16 timeout_ms);
extern U8 C2_Poll_OTPBusy (U16 timeout_ms);

extern U8 C2_Halt (void);
extern U8 C2_GetDevID (U8 *devid);
extern U8 C2_GetRevID (U8 *revid);
extern U8 C2_ReadSFR (U8 sfraddress, U8 *sfrdata);
extern U8 C2_WriteSFR (U8 sfraddress, U8 sfrdata);

extern U8 C2_WriteCommand (U8 command, U16 C2_poll_inbusy_timeout_ms);
extern U8 C2_ReadResponse (U16 C2_poll_outready_timeout_ms);
extern U8 C2_ReadData (U16 C2_poll_outready_timeout_ms);

extern U8 C2_ReadDirect (U8 sfraddress, U8 *sfrdata, U8 indirect);
extern U8 C2_WriteDirect (U8 sfraddress, U8 sfrdata, U8 indirect);

extern U8 C2_Discover (U8* deviceid, U8* derid);
extern U8 C2_GetPageCRC(U8 page, U16 *crc);

U8 C2_Step();

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------

#define C2_AR_OUTREADY 0x01
#define C2_AR_INBUSY   0x02
#define C2_AR_OTPBUSY  0x80
#define C2_AR_OTPERROR 0x40
#define C2_AR_FLBUSY   0x80

#define C2_DEVICEID    0x00
#define C2_REVID       0x01
#define C2_FPCTL       0x02

#define C2_FPCTL_RUNNING    0x00
#define C2_FPCTL_HALT       0x01
#define C2_FPCTL_RESET      0x02
#define C2_FPCTL_CORE_RESET 0x04

//#define C2_FPDAT                0xB4
#define C2_FPDAT_GET_VERSION    0x01
#define C2_FPDAT_GET_DERIVATIVE 0x02
#define C2_FPDAT_DEVICE_ERASE   0x03
#define C2_FPDAT_GET_CRC		0x05
#define C2_FPDAT_BLOCK_READ     0x06
#define C2_FPDAT_BLOCK_WRITE    0x07
#define C2_FPDAT_PAGE_ERASE     0x08
#define C2_FPDAT_DIRECT_READ    0x09
#define C2_FPDAT_DIRECT_WRITE   0x0a
#define C2_FPDAT_INDIRECT_READ  0x0b
#define C2_FPDAT_INDIRECT_WRITE 0x0c

#define C2_FPDAT_RETURN_INVALID_COMMAND 0x00
#define C2_FPDAT_RETURN_COMMAND_FAILED  0x02
#define C2_FPDAT_RETURN_FLASH_ERR       0x03
#define C2_FPDAT_RETURN_NO_HALT_ERR     0x0B
#define C2_FPDAT_RETURN_COMMAND_OK      0x0D

#define C2_DEVCTL             0x02
#define C2_EPCTL              0xDF
#define C2_EPDAT              0xBF
#define C2_EPADDRH            0xAF
#define C2_EPADDRL            0xAE
#define C2_EPSTAT             0xB7
#define C2_EPSTAT_WRITE_LOCK  0x80
#define C2_EPSTAT_READ_LOCK   0x40
#define C2_EPSTAT_CAL_VALID   0x20
#define C2_EPSTAT_CAL_DONE    0x10
#define C2_EPSTAT_ERROR       0x01

#define C2_EPCTL_READ         0x00
#define C2_EPCTL_WRITE1       0x40
#define C2_EPCTL_WRITE2       0x58
#define C2_EPCTL_FAST_WRITE   0x78

// C2 DR timeouts (us)
#define C2_WRITE_DR_TIMEOUT_US 10000
#define C2_READ_DR_TIMEOUT_US 10000

// C2 Debug timeouts (ms)
#define C2_POLL_INBUSY_TIMEOUT_MS 10
#define C2_POLL_OUTREADY_TIMEOUT_MS 10
#define C2_POLL_OTPBUSY_TIMEOUT_MS 10

enum {C2_DIRECT, C2_INDIRECT};

//-----------------------------------------------------------------------------
// Exported macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

extern U8 C2_AR;
extern U8 C2_DR;

#endif // C2_UTILS_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

