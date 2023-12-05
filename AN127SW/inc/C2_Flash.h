//-----------------------------------------------------------------------------
// C2_Flash.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for accessing the Flash or OTP memory
// using the Silicon Labs C2 interface.
//
//-----------------------------------------------------------------------------

#ifndef C2_FLASH_H
#define C2_FLASH_H

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern U8 C2_FLASH_Read (S8 *dest, U32 addr, U16 length);
extern U8 C2_OTP_Read (S8 *dest, U32 addr, U16 length);
extern U8 C2_FLASH_Write (U32 addr, S8 *src, U16 length);
extern U8 C2_OTP_Write (U32 addr, S8 *src, U16 length);
extern U8 C2_FLASH_PageErase (U32 addr);
extern U8 C2_FLASH_DeviceErase (void);
extern U8 C2_FLASH_BlankCheck (U32 addr, U32 length);
extern U8 C2_OTP_BlankCheck (U32 addr, U32 length);
extern U8 C2_Get_LockByte (void);

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

#endif // C2_FLASH_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

