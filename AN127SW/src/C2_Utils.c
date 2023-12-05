//-----------------------------------------------------------------------------
// C2_Utils.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains low-level C2 interface routines used by C2_Flash.c.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Common.h"
#include "Timing.h"
#include "Pins.h"
#include "C2_Utils.h"
#include "C2_Flash.h"
#include "Errors.h"
#include "Devices.h"
#include <stdlib.h>

#include <stdio.h>

//-----------------------------------------------------------------------------
// Exported Constants
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

U8 C2_Reset (void);
U8 C2_WriteAR (U8 addr);
U8 C2_ReadAR (void);
U8 C2_WriteDR (U8 dat, U16 timeout_us);
U8 C2_ReadDR (U16 timeout_us);

U8 C2_Poll_InBusy (U16 timeout_ms);
U8 C2_Poll_OutReady (U16 timeout_ms);
U8 C2_Poll_OTPBusy (U16 timeout_ms);

U8 C2_Halt (void);
U8 C2_GetDevID (U8 *devid);
U8 C2_GetRevID (U8 *revid);
U8 C2_ReadSFR (U8 sfraddress, U8 *sfrdata);
U8 C2_WriteSFR (U8 sfraddress, U8 sfrdata);

U8 C2_WriteCommand (U8 command, U16 C2_poll_inbusy_timeout_ms);
U8 C2_ReadResponse (U16 C2_poll_outready_timeout_ms);
U8 C2_ReadData (U16 C2_poll_outready_timeout_ms);

U8 C2_ReadDirect (U8 sfraddress, U8 *sfrdata, U8 indirect);
U8 C2_WriteDirect (U8 sfraddress, U8 sfrdata, U8 indirect);

U8 C2_GetPageCRC(U8 page, U16 *crc);
U8 C2_Discover (U8* deviceid, U8* derid);

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------


#if __C51__
SEGMENT_VARIABLE (C2_AR, U8, SEG_BDATA);
SEGMENT_VARIABLE (C2_DR, U8, SEG_BDATA);

SBIT (AR0, C2_AR, 0);
SBIT (AR1, C2_AR, 1);
SBIT (AR2, C2_AR, 2);
SBIT (AR3, C2_AR, 3);
SBIT (AR4, C2_AR, 4);
SBIT (AR5, C2_AR, 5);
SBIT (AR6, C2_AR, 6);
SBIT (AR7, C2_AR, 7);

SBIT (DR0, C2_DR, 0);
SBIT (DR1, C2_DR, 1);
SBIT (DR2, C2_DR, 2);
SBIT (DR3, C2_DR, 3);
SBIT (DR4, C2_DR, 4);
SBIT (DR5, C2_DR, 5);
SBIT (DR6, C2_DR, 6);
SBIT (DR7, C2_DR, 7);
#else
U8 C2_AR;                        // C2 Address register
U8 C2_DR;                        // C2 Data register
#endif



//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

// C2_Discover Helper Functions
U8 findFamilyFromDeviceId(U8 devid, DEVICE_FAMILY **dfptr);
bit findDerivativeFromDerivId(U16 derivativeid, DEVICE_FAMILY const * const dfptr, DERIVATIVE_ENTRY const **deptr);

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C2 Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C2_Reset
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Performs a target device reset by pulling the C2CK pin low for >20us.
//
//-----------------------------------------------------------------------------
U8 C2_Reset (void)
{
   C2CK_DRIVER_ON ();
   C2CK = LOW;                         // Put target device in reset state
   Wait_us (20);                       // by driving C2CK low for >20us

   C2CK = HIGH;                        // Release target device from reset
   Wait_us (2);                        // wait at least 2us before Start
   C2CK_DRIVER_OFF ();

   FAMILY_FOUND = FALSE;
   DERIVATIVE_FOUND = FALSE;
   DEVICE_HALTED = FALSE;

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// C2_WriteAR
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : value for address
//
// Performs a C2 Address register write (writes the <addr> input to Address
// register).
//
//-----------------------------------------------------------------------------
U8 C2_WriteAR (U8 addr)
{
#ifndef __C51__
   U8 i;
#endif
   static bit EA_SAVE;                 // preserve interrupts

   EA_SAVE = EA;
   DISABLE_INTERRUPTS ();              // disable interrupts

   C2CK_DRIVER_ON ();

   // START field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   // INS field (11b, LSB first)
   C2D_DRIVER_ON ();                   // Enable C2D driver (output)
   C2D = HIGH;
   STROBE_C2CK ();
   C2D = HIGH;
   STROBE_C2CK ();

#ifdef __C51__
   // send 8-bit address
   C2_AR = addr;

   C2D = AR0; STROBE_C2CK ();
   C2D = AR1; STROBE_C2CK ();
   C2D = AR2; STROBE_C2CK ();
   C2D = AR3; STROBE_C2CK ();
   C2D = AR4; STROBE_C2CK ();
   C2D = AR5; STROBE_C2CK ();
   C2D = AR6; STROBE_C2CK ();
   C2D = AR7; STROBE_C2CK ();
#else
   // send 8-bit address
   for (i = 0; i < 8; i++)             // Shift the 8-bit addr field
   {
      if (addr & 0x01)                 // present LSB of addr at C2D pin
      {
         C2D = HIGH;
      }
      else
      {
         C2D = LOW;
      }

      STROBE_C2CK ();

      addr = addr >> 1;
   }
#endif
   // STOP field
   C2D_DRIVER_OFF ();                  // Disable C2D driver
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   C2CK_DRIVER_OFF ();

   EA = EA_SAVE;                       // Restore interrupts

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// C2_ReadAR
//-----------------------------------------------------------------------------
//
// Return Value : Error code, C2_AR global variable
// Parameters   : None
//
// Performs a C2 Address register read.
// Returns the 8-bit register content in global variable 'C2_AR'.  Return
// value is an error code.
//
//-----------------------------------------------------------------------------
U8 C2_ReadAR (void)
{
#ifndef __C51__
   U8 i;                               // bit counter
   U8 addr;                            // value read from C2 interface
#endif
   static bit EA_SAVE;                 // preserve EA

   EA_SAVE = EA;

   DISABLE_INTERRUPTS ();              // disable interrupts

   C2CK_DRIVER_ON ();

   // START field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   // INS field (10b, LSB first)
   C2D = LOW;
   C2D_DRIVER_ON ();                   // Enable C2D driver (output)
   STROBE_C2CK ();
   C2D = HIGH;
   STROBE_C2CK ();

   C2D_DRIVER_OFF ();                  // Disable C2D driver (input)

#ifdef __C51__
   // read 8-bit ADDRESS field
   STROBE_C2CK (); AR0 = C2D;
   STROBE_C2CK (); AR1 = C2D;
   STROBE_C2CK (); AR2 = C2D;
   STROBE_C2CK (); AR3 = C2D;
   STROBE_C2CK (); AR4 = C2D;
   STROBE_C2CK (); AR5 = C2D;
   STROBE_C2CK (); AR6 = C2D;
   STROBE_C2CK (); AR7 = C2D;
#else
   // read 8-bit ADDRESS field
   addr = 0;
   for (i = 0; i < 8; i++)             // Shift in 8 bit ADDRESS field
   {                                   // LSB-first
      addr >>= 1;
      STROBE_C2CK ();
      if (C2D == HIGH)
      {
         addr |= 0x80;
      }
   }
   C2_AR = addr;                       // update global variable with result
#endif
   // STOP field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   C2CK_DRIVER_OFF ();                 // turn off C2CK driver


   EA = EA_SAVE;                       // restore interrupts

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// C2_WriteDR
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : data to write to C2
//
// Performs a C2 Data register write (writes <dat> input to data register).
// Requires Timeout_us_1 to be initialized prior to call.
//
//-----------------------------------------------------------------------------
U8 C2_WriteDR (U8 dat, U16 timeout_us)
{
#ifndef __C51__
   U8 i;                               // bit counter
#endif
   static bit EA_SAVE;                 // preserve interrupts
   U8 return_value;

   EA_SAVE = EA;

   DISABLE_INTERRUPTS ();              // disable interrupts

   return_value = NO_ERROR;

   C2CK_DRIVER_ON ();

   // START field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   // INS field (01b, LSB first)
   C2D = HIGH;
   C2D_DRIVER_ON ();                   // Enable C2D driver
   STROBE_C2CK ();
   C2D = LOW;
   STROBE_C2CK ();

   // LENGTH field (00b -> 1 byte)
   C2D = LOW;
   STROBE_C2CK ();
   C2D = LOW;
   STROBE_C2CK ();

#ifdef __C51__
   // write 8-bit DATA field
   C2_DR = dat;
   C2D = DR0; STROBE_C2CK ();
   C2D = DR1; STROBE_C2CK ();
   C2D = DR2; STROBE_C2CK ();
   C2D = DR3; STROBE_C2CK ();
   C2D = DR4; STROBE_C2CK ();
   C2D = DR5; STROBE_C2CK ();
   C2D = DR6; STROBE_C2CK ();
   C2D = DR7; STROBE_C2CK ();
#else
   // write 8-bit DATA field
   for (i = 0; i < 8; i++)             // Shift out 8-bit DATA field
   {                                   // LSB-first
      if (dat & 0x01)
      {
         C2D = HIGH;
      }
      else
      {
         C2D = LOW;
      }

      STROBE_C2CK ();
      dat >>= 1;
   }
#endif

   // WAIT field
   C2D_DRIVER_OFF ();                  // Disable C2D driver for input

   STROBE_C2CK ();                     // clock first wait field

   Set_Timeout_us_1 (timeout_us);

   while ((C2D == LOW) && (!Timeout_us_1))
   {
      STROBE_C2CK ();                  // sample Wait until it returns High
                                       // or until timeout expires
   }

   if ((C2D == LOW) && (Timeout_us_1 == 1))
   {
      return_value = C2DR_WRITE_TIMEOUT;
   }

   // STOP field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   C2CK_DRIVER_OFF ();

   EA = EA_SAVE;                       // restore interrupts

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_ReadDR
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Performs a C2 Data register read.
// Returns the 8-bit register content in global C2_DR.
// Requires Timeout_us_1 to be initialized prior to call.
//
//-----------------------------------------------------------------------------
U8 C2_ReadDR (U16 timeout_us)
{
#ifndef __C51__
   U8 i;                               // bit counter
   U8 dat;                             // data holding register
#endif
   U8 return_value;

   static bit EA_SAVE;                 // preserve interrupts

   EA_SAVE = EA;

   DISABLE_INTERRUPTS ();              // disable interrupts

   return_value = NO_ERROR;

   C2CK_DRIVER_ON ();

   // START field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   // INS field (00b, LSB first)
   C2D_DRIVER_ON ();                   // Enable C2D driver (output)
   C2D = LOW;
   STROBE_C2CK ();
   C2D = LOW;
   STROBE_C2CK ();

   // LENGTH field (00b -> 1 byte)
   C2D = LOW;
   STROBE_C2CK ();
   C2D = LOW;
   STROBE_C2CK ();

   // WAIT field
   C2D_DRIVER_OFF ();                  // Disable C2D driver for input

   Set_Timeout_us_1 (timeout_us);

   STROBE_C2CK ();                     // sample first Wait cycle

   while ((C2D == 0) && (!Timeout_us_1))
   {
      STROBE_C2CK ();                  // stobe C2CK until Wait returns '1' or
                                       // timeout
   }

   if ((C2D == LOW) && (Timeout_us_1 == 1))
   {
      return_value = C2DR_READ_TIMEOUT;
   }
#ifdef __C51__
   STROBE_C2CK (); DR0 = C2D;
   STROBE_C2CK (); DR1 = C2D;
   STROBE_C2CK (); DR2 = C2D;
   STROBE_C2CK (); DR3 = C2D;
   STROBE_C2CK (); DR4 = C2D;
   STROBE_C2CK (); DR5 = C2D;
   STROBE_C2CK (); DR6 = C2D;
   STROBE_C2CK (); DR7 = C2D;
#else
   // 8-bit DATA field
   dat = 0;
   for (i = 0; i < 8; i++)             // Shift in 8-bit DATA field
   {                                   // LSB-first
      dat >>= 1;
      STROBE_C2CK ();
      if (C2D == HIGH)
      {
         dat  |= 0x80;
      }
   }
   C2_DR = dat;                        // update global C2 DAT value
#endif

   // STOP field
   STROBE_C2CK ();                     // Strobe C2CK with C2D driver disabled

   C2CK_DRIVER_OFF ();                 // turn off C2CK driver


   EA = EA_SAVE;                       // restore interrupts

   return return_value;
}


//-----------------------------------------------------------------------------
// C2_Poll_InBusy
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function polls the INBUSY flag of C2_AR.
//
//-----------------------------------------------------------------------------
U8 C2_Poll_InBusy (U16 timeout_ms)
{
   U8 return_value;

   Set_Timeout_ms_1 (timeout_ms);

   return_value = NO_ERROR;

   while (1)
   {
      C2_ReadAR ();
      if ((C2_AR & C2_AR_INBUSY) == 0)
      {
         break;                        // exit on NO_ERROR and not busy
      }
      if (Timeout_ms_1)
      {
         // exit on POLL_INBUSY timeout
         return_value = C2_POLL_INBUSY_TIMEOUT;
         break;
      }
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_Poll_OutReady
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function polls the OUTREADY flag of C2_AR.
//
//-----------------------------------------------------------------------------
U8 C2_Poll_OutReady (U16 timeout_ms)
{
   U8 return_value;

   Set_Timeout_ms_1 (timeout_ms);

   return_value = NO_ERROR;

   while (1)
   {
      C2_ReadAR ();

      if (C2_AR & C2_AR_OUTREADY)
      {
         break;                        // exit on NO_ERROR and data ready
      }
      if (Timeout_ms_1)
      {
         // exit on POLL_INBUSY timeout
         return_value = C2_POLL_OUTREADY_TIMEOUT;
         break;
      }
   }

   return return_value;
}


//-----------------------------------------------------------------------------
// C2_Halt
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine issues a pin reset of the device, followed by a device
// reset, core reset, then Halt request.
//
//-----------------------------------------------------------------------------
U8 C2_Halt (void)
{
   U8 return_value;

   C2_Reset ();                        // issue pin reset

   // issue standard reset
   C2_WriteAR (C2_FPCTL);


   if (return_value = C2_WriteDR (C2_FPCTL_RESET, C2_WRITE_DR_TIMEOUT_US))
   {
      return return_value;
   }

   // issue core reset
   if (return_value = C2_WriteDR (C2_FPCTL_CORE_RESET, C2_WRITE_DR_TIMEOUT_US))
   {
      return return_value;
   }

   // issue HALT
   if (return_value = C2_WriteDR (C2_FPCTL_HALT, C2_WRITE_DR_TIMEOUT_US))
   {
      return return_value;
   }

   Wait_ms (20);                       // wait at least 20 ms

   DEVICE_HALTED = TRUE;

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_GetDevID
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine returns the device family ID by reference to <devid>.
//
//-----------------------------------------------------------------------------
U8 C2_GetDevID (U8 *devid)
{
   U8 return_value;
   U8 temp;

   if (return_value = C2_ReadSFR (C2_DEVICEID, &temp))
   {
      return return_value;
   }

   *devid = temp;

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_GetRevID
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine returns the device revision ID by reference to <revid>.
//
//-----------------------------------------------------------------------------
U8 C2_GetRevID (U8 *revid)
{
   U8 return_value;
   U8 temp;

   if (return_value = C2_ReadSFR (C2_REVID, &temp))
   {
      return return_value;
   }

   *revid = temp;

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_ReadSFR
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : sfraddress contains address to read, sfrdata points to
//                a result holding register
//
// This routine reads the contents of the SFR at address <sfraddress> and
// returns its value by reference into <sfrdata>.
//
//-----------------------------------------------------------------------------
U8 C2_ReadSFR (U8 sfraddress, U8 *sfrdata)
{
   U8 return_value;

   C2_WriteAR (sfraddress);

   if (return_value = C2_ReadDR (C2_READ_DR_TIMEOUT_US))
   {
      return SFR_READ_TIMEOUT;
   }

   *sfrdata = C2_DR;

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_WriteSFR
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : sfraddress contains address to write, sfrdata contains data
//                to write
//
// This routine writes the contents of the SFR at address <sfraddress> to
// the value of <sfrdata>.
//
//-----------------------------------------------------------------------------
U8 C2_WriteSFR (U8 sfraddress, U8 sfrdata)
{
   U8 return_value;

   C2_WriteAR (sfraddress);

   if (return_value = C2_WriteDR (sfrdata, C2_WRITE_DR_TIMEOUT_US))
   {
      return SFR_WRITE_TIMEOUT;
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_WriteCommand
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : command
//
// This routine writes a C2 Flash command (WriteDR + INBUSY polling).
//
//-----------------------------------------------------------------------------
U8 C2_WriteCommand (U8 command, U16 C2_poll_inbusy_timeout_ms)
{
   U8 return_value;

   // Send command
   if (return_value = C2_WriteDR (command, C2_WRITE_DR_TIMEOUT_US))
   {
      return return_value;
   }

   // verify acceptance
   if (return_value = C2_Poll_InBusy (C2_poll_inbusy_timeout_ms))
   {
      return return_value;
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_ReadResponse
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This routine reads a C2 Flash command response (Outready polling + ReadDR).
//
//-----------------------------------------------------------------------------
U8 C2_ReadResponse (U16 C2_poll_outready_timeout_ms)
{
   U8 return_value;

   // check for response
   if (return_value = C2_Poll_OutReady (C2_poll_outready_timeout_ms))
   {
      return return_value;
   }

   // read status
   if (return_value = C2_ReadDR (C2_READ_DR_TIMEOUT_US))
   {
      return return_value;
   }

   if (C2_DR != COMMAND_OK)
   {
      if (C2_DR == C2_FPDAT_RETURN_INVALID_COMMAND)
      {
         return BAD_DEBUG_COMMAND;
      }
      else
      {
         return C2_DR;
      }
   }

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// C2_ReadData
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This routine reads a C2 Flash command data byte (Outready polling + ReadDR).
//
//-----------------------------------------------------------------------------
U8 C2_ReadData (U16 C2_poll_outready_timeout_ms)
{
   U8 return_value;

   // check for response
   if (return_value = C2_Poll_OutReady (C2_poll_outready_timeout_ms))
   {
      return return_value;
   }

   // read status
   if (return_value = C2_ReadDR (C2_READ_DR_TIMEOUT_US))
   {
      return return_value;
   }

   return NO_ERROR;
}

U8 C2_Step()
{
  U8 return_value;
  //C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);
  return_value = C2_WriteCommand (0x26, C2_POLL_INBUSY_TIMEOUT_MS);

  //return_value = C2_WriteDR(0x26, C2_POLL_INBUSY_TIMEOUT_MS);
  return return_value;

}
//-----------------------------------------------------------------------------
// C2_ReadDirect
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : sfraddress contains address to read, sfrdata points to
//                a result holding register
//
// This routine reads the contents of the SFR at address <sfraddress> and
// returns its value by reference into <sfrdata>.
// If <address> is 0x00 to 0x7f, this function accesses RAM.
// Requires that FPDAT is known and the device is Halted.
// If <indirect> is "C2_INDIRECT", <address> targets RAM for all addresses.
//
//-----------------------------------------------------------------------------
U8 C2_ReadDirect (U8 sfraddress, U8 *sfrdata, U8 indirect)
{
   U8 return_value;

   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   // set up command accesses
   if (indirect == C2_DIRECT)
   {
      if (return_value = C2_WriteCommand (C2_FPDAT_DIRECT_READ, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }
   }
   else
   {
      if (return_value = C2_WriteCommand (C2_FPDAT_INDIRECT_READ, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }
   }


   // Check command status
   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now send address of byte to read
   if (return_value = C2_WriteCommand (sfraddress, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // Send number of bytes to read
   if (return_value = C2_WriteCommand (1, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // read the data
   if (return_value = C2_ReadData (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   *sfrdata = C2_DR;

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_WriteDirect
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : sfraddress contains address to write, sfrdata contains data
//                to write
//
// This routine writes <sfrdata> to address <sfraddress>.
// If <address> is 0x00 to 0x7f, this function accesses RAM.
// Requires that FPDAT is known and the device is Halted.
// If <indirect> is "C2_INDIRECT", <address> targets RAM for all addresses.
//
//-----------------------------------------------------------------------------
U8 C2_WriteDirect (U8 sfraddress, U8 sfrdata, U8 indirect)
{
   U8 return_value;

   // set up command accesses
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   if (indirect == C2_DIRECT)
   {
      if (return_value = C2_WriteCommand (C2_FPDAT_DIRECT_WRITE, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }
   }
   else
   {
      if (return_value = C2_WriteCommand (C2_FPDAT_INDIRECT_WRITE, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }
   }

   // Check response
   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now send address of byte to write
   if (return_value = C2_WriteCommand (sfraddress, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // Send number of bytes to write
   if (return_value = C2_WriteCommand (1, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // Send the data
   if (return_value = C2_WriteCommand (sfrdata, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// findDerivativeFromDerivId
//-----------------------------------------------------------------------------
//
// Return Value : bool - derivative found
// Parameters   : derivative ID, dfptr, deptr
//
// Helper function for C2_Discover
//
// This routine searches the given family (dfptr) for a given derivative ID,
// and then sets the derivative pointer (deptr) to this derivative structure.
// If the derivative ID doesn't exist in the family, this function returns
// False, otherwise True.
//
//-----------------------------------------------------------------------------
bit findDerivativeFromDerivId(U16 derivativeid, DEVICE_FAMILY const * const dfptr, DERIVATIVE_ENTRY const **deptr)
{
   U8 derivative_number = 0;
   bit derivative_found = FALSE;
   *deptr = &(dfptr->DERIVATIVE_LIST[0]);
   while (((*deptr)->DERIVATIVE_STRING != NULL ))
   {
      if (derivativeid == (*deptr)->DERIVATIVE_ID)
      {
         DERIVATIVE_NUMBER = derivative_number;
         derivative_found = TRUE;
         break;
      }
      else
      {
         derivative_number++;
         *deptr = &(dfptr->DERIVATIVE_LIST[derivative_number]);
      }
   }
   return derivative_found;
}

//-----------------------------------------------------------------------------
// findFamilyFromDeviceId
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : derivative ID, dfptr, deptr
//
// Helper function for C2_Discover
//
// This routine searches the family list for the given device ID, and sets the
// family pointer (dfptr) to point to the correct family, if it exists.
//
// Returns FAMILY_NOT_SUPPORTED if the family isn't found within the list,
// NO_ERROR if it is found.
//
//-----------------------------------------------------------------------------
U8 findFamilyFromDeviceId(U8 devid, DEVICE_FAMILY **dfptr)
{
	static U8 family_number = 0;
	if (devid != KNOWN_FAMILIES[FAMILY_NUMBER].DEVICE_ID) {
		family_number = 0;
	}

	FAMILY_FOUND = FALSE;
	while ((FAMILY_FOUND == FALSE)
			&& (KNOWN_FAMILIES[family_number].FAMILY_STRING != NULL ))
	{
		// If the devid matches the devid for the current family, and the dfptr
		// isn't already set to this family, we've found a new family that is
		// compatible
		if ((devid == KNOWN_FAMILIES[family_number].DEVICE_ID)
				&& (*dfptr != (DEVICE_FAMILY *) &(KNOWN_FAMILIES[family_number])))
		{
			FAMILY_FOUND = TRUE;
			FAMILY_NUMBER = family_number;
			break;
		}
		else
		{
			family_number++;
		}
	}

	if (FAMILY_FOUND == FALSE)
	{
		return FAMILY_NOT_SUPPORTED;
	}

	*dfptr = (DEVICE_FAMILY *) &(KNOWN_FAMILIES[FAMILY_NUMBER]);
	return NO_ERROR;
}

//-----------------------------------------------------------------------------
// C2_Discover
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This routine will eventually be replaced by C2_Discover().
//
// This routine queries the Device ID and performs a lookup to get the
// FPDAT register address for the target device family.  Assigns
// the global variables 'FAMILY_NUMBER', 'DERIVATIVE_NUMBER',
// 'DEVICE_REV', and 'DERIVATIVE_REV'.
//
//-----------------------------------------------------------------------------
U8 C2_Discover (U8* deviceid, U8* derid)
{
   U8 return_value;
   U8 devid;
   U16 derivativeid;
   DEVICE_FAMILY *dfptr = (DEVICE_FAMILY*)0;
   DERIVATIVE_ENTRY *deptr;

   return_value = C2_GetDevID (&devid);
   *deviceid = devid;
   if (return_value != NO_ERROR)
   {
      return return_value;
   }

   return_value = C2_GetRevID (&DEVICE_REV);
   if (return_value != NO_ERROR)
   {
      return return_value;
   }

   return_value = findFamilyFromDeviceId(devid, &dfptr);
   if (return_value != NO_ERROR)
   {
	   return return_value;
   }

   C2_WriteAR (dfptr->FPDAT);

   if (return_value = C2_WriteCommand (C2_FPDAT_GET_DERIVATIVE, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now read the derivative value
   if (return_value = C2_ReadData (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   derivativeid = (U16) C2_DR;
   *derid = derivativeid;

   do
   {
      DERIVATIVE_FOUND = findDerivativeFromDerivId(derivativeid, dfptr, &deptr);
      
      if (DERIVATIVE_FOUND == TRUE)
      {
         break;
      } 
      else 
      {
         findFamilyFromDeviceId(devid, &dfptr);
      }
      
   } while (FAMILY_FOUND == TRUE);

   if (DERIVATIVE_FOUND == FALSE)
   {
      return_value = DERIVATIVE_NOT_SUPPORTED;
      return return_value;
   }

   // Get derivative version

   C2_WriteAR (dfptr->FPDAT);

   if (return_value = C2_WriteCommand (C2_FPDAT_GET_VERSION, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now read the derivative value
   if (return_value = C2_ReadData (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   DERIVATIVE_REV = (U16) C2_DR;


   return return_value;
}

//-----------------------------------------------------------------------------
// C2_GetPageCRC
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : block, crc
//
// This routine calculates the CRC of the given 256-byte block of flash memory.
//
//-----------------------------------------------------------------------------
U8 C2_GetPageCRC(U8 block, U16 *crc)
{
	U8 return_value;
	C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

	if (return_value = C2_WriteCommand(C2_FPDAT_GET_CRC, C2_POLL_INBUSY_TIMEOUT_MS)) {
		return return_value;
	}

	if (return_value = C2_ReadResponse(C2_POLL_OUTREADY_TIMEOUT_MS)) {
		return return_value;
	}

	if (return_value = C2_WriteCommand(block, C2_POLL_INBUSY_TIMEOUT_MS)) {
		return return_value;
	}

	// read CRC MSB
	if (return_value = C2_ReadData(C2_POLL_OUTREADY_TIMEOUT_MS)) {
		return return_value;
	}

	*crc = C2_DR << 8;

	// read CRC LSB
	if (return_value = C2_ReadData(C2_POLL_OUTREADY_TIMEOUT_MS)) {
		return return_value;
	}

	*crc |= C2_DR;

	return return_value;
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

