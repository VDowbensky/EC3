//-----------------------------------------------------------------------------
// Devices.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains JTAG/C2 device information.
//
//-----------------------------------------------------------------------------

#ifndef DEVICES_H
#define DEVICES_H

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Exported Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

enum MEM_TYPE {FLASH = 0, OTP, ROM};
enum IF_TYPE {JTAG = 0, C2};
enum SECURITY_TYPE {JTAG_SECURITY = 0, C2_1, C2_2, C2_3};

typedef U8 * INIT_STRING;

// DERIVATIVE_ID            FEATURES          CODE_START      WRITELOCK     CODE2_START
//        DERIVATIVE_STRING           PACKAGE        CODE_SIZE       READLOCK          CODE2_SIZE

typedef struct DERIVATIVE_ENTRY
{
   U16 DERIVATIVE_ID;                  // Specific Derivative ID
   U8 *DERIVATIVE_STRING;              // Specific Part number
   U8 *FEATURES_STRING;                // String listing specific features
   U8 *PACKAGE_STRING;                 // String listing package info
   U32 CODE_START;                     // starting code address
   U32 CODE_SIZE;                      // size of code space
   U32 WRITELOCKBYTEADDR;              // address of Write Lock Byte
   U32 READLOCKBYTEADDR;               // address of Read Lock Byte
   U32 CODE2_START;                    // starting address of 2nd code bank
   U32 CODE2_SIZE;                     // size of code2 space
} DERIVATIVE_ENTRY;

// DEVICE_ID            MEM_TYPE           PAGE_SIZE      SECURITY_TYPE    FLASHSCALE         FPDAT    DERIVATIVE list
//        FAMILY_STRING           IF_TYPE        HAS_SFLE           STATCTL          FLA_LEN       INIT

typedef struct DEVICE_FAMILY
{
   U16 DEVICE_ID;                      // C2/JTAG family ID
   U8 *FAMILY_STRING;                  // name of device family
   U8 MEM_TYPE;                        // FLASH or OTP
   U16 PAGE_SIZE;                      // number of bytes per code page
   U8 HAS_SFLE;                        // TRUE if device has SFLE bit
   U8 SECURITY_TYPE;                   // Flash security type
   U8 FPDAT;                           // Flash Programming Data Register
                                       //  address
   INIT_STRING *INIT_STRINGS;          // List of initialization commands
   DERIVATIVE_ENTRY *DERIVATIVE_LIST;  // List of derivatives for this family
} DEVICE_FAMILY;


//-----------------------------------------------------------------------------
// Exported Global Variables
//-----------------------------------------------------------------------------

//extern DEVICE_ENTRY code KNOWN_DEVICES[];
extern DEVICE_FAMILY code KNOWN_FAMILIES[];

extern SEGMENT_VARIABLE (FAMILY_NUMBER, U8, SEG_PDATA);
extern SEGMENT_VARIABLE (DERIVATIVE_NUMBER, U8, SEG_PDATA);
extern SEGMENT_VARIABLE (DEVICE_REV, U8, SEG_PDATA);
extern SEGMENT_VARIABLE (DERIVATIVE_REV, U8, SEG_PDATA);

extern bit FAMILY_FOUND;
extern bit DERIVATIVE_FOUND;
extern bit DEVICE_HALTED;

#endif // DEVICES_H


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

