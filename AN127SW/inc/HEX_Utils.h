//-----------------------------------------------------------------------------
// HEX_Utils.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains several useful utilities for interfacing encoding and
// decoding HEX records
//
//-----------------------------------------------------------------------------

#include "Common.h"

#ifndef HEX_UTILS
#define HEX_UTILS

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"

//-----------------------------------------------------------------------------
// Exported Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

typedef struct
{
   U8 RECLEN;
   UU16 OFFSET;
   U8 RECTYP;
   U8 *Buf;
} HEX_RECORD;

//-----------------------------------------------------------------------------
// Exported Global Constants
//-----------------------------------------------------------------------------

#define HEX_RECLEN 32                  // number of data bytes per generated
                                       // HEX record

#define HEXREC_DAT 0                   // Data Record
#define HEXREC_EOF 1                   // End of File Record
#define HEXREC_EXA 2                   // Extended Segment Address Record
#define HEXREC_SSA 3                   // Start Segment Address Record
#define HEXREC_ELA 4                   // Extended Linear Address Record
#define HEXREC_SLA 5                   // Start Linear Address Record

//-----------------------------------------------------------------------------
// Exported Prototypes
//-----------------------------------------------------------------------------

extern U8 HEX_Encode (S8 *dest, HEX_RECORD *hexsrc, bit suppress);
extern U8 HEX_Decode (HEX_RECORD *hexdest, S8 *src);
extern U8 HEX2uc (S8 *src);
extern void uc2HEX (S8 *dest, U8 mychar);
extern void HEXSTR2BIN (S8 *dest, S8 *src, U8 *length);
extern void BIN2HEXSTR (S8 *dest, S8 *src, U8 length);

#endif // HEX_UTILS

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

