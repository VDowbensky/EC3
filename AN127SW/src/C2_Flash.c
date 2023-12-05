//-----------------------------------------------------------------------------
// C2_Flash.c
//-----------------------------------------------------------------------------
// Copyright (C) 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains routines for accessing the Flash or OTP memory using
// the Silicon Labs C2 interface.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include <stdlib.h>
#include "Common.h"
#include "Timing.h"
#include "Pins.h"
#include "C2_Utils.h"
#include "C2_Flash.h"
#include "Devices.h"
#include "Errors.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

// If neither of the following (2) constants is defined, OTP error checking
// is disabled as a compile option (to improve speed).
//#define OTP_ERROR_CHECK_ADDRESS        // if defined, error checking is enabled
                                       // for OTP reads and writes and is performed
                                       // on the current OTP address prior to
                                       // the OTP operation
//#define OTP_ERROR_CHECK_OPERATION      // if defined, error checking is enabled
                                       // for OTP reads and writes and is performed
                                       // after the OTP operation (if the operation
                                       // failed, it is noted)

#define OTP_READ_TIMEOUT_US  100
#define OTP_WRITE_TIMEOUT_US 300
#define OTP_VPP_SETTLING_TIME_US 150
#define FLASH_READ_TIMEOUT_MS 10
#define FLASH_WRITE_TIMEOUT_MS 10
#define FLASH_PAGE_ERASE_TIMEOUT_MS 40
#define FLASH_DEVICE_ERASE_TIMEOUT_MS 20000

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

uint8_t C2_FLASH_Read (int8_t *dest, uint32_t addr, uint16_t length);
uint8_t C2_OTP_Read (int8_t *dest, uint32_t addr, uint16_t length);
uint8_t C2_FLASH_Write (uint32_t addr, int8_t *src, uint16_t length);
uint8_t C2_OTP_Write (uint32_t addr, int8_t *src, uint16_t length);
uint8_t C2_FLASH_PageErase (uint32_t addr);
uint8_t C2_FLASH_DeviceErase (void);
uint8_t C2_FLASH_BlankCheck (uint32_t addr, uint32_t length);
uint8_t C2_OTP_BlankCheck (uint32_t addr, uint32_t length);
uint8_t C2_Get_LockByte (void);

//-----------------------------------------------------------------------------
// C2 Flash Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C2_FLASH_Read
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Reads a block of FLASH memory of <length> starting at <addr> and
// copies it to the buffer pointed to by <dest>.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
// Note: for 128K devices, addresses cannot cross bank boundaries
//
//-----------------------------------------------------------------------------
uint8_t C2_FLASH_Read (int8_t *dest, uint32_t addr, uint16_t length)
{
   uint8_t return_value;

   uint16_t i;// byte counter
   uint16_t blocksize;// size of this block transfer
   uint8_t psbank_val;
   UU16 C2_addr;// address in C2 space
   uint8_t psbank_addr;// address of PSBANK SFR (Todo: move to Devices.c)
   uint8_t device_id;// device_id of target (for psbank maintenance)

   return_value = NO_ERROR;
   // set up "psbank_addr" for 128K devices ('F58x and 'F96x)
   device_id = KNOWN_FAMILIES[FAMILY_NUMBER].DEVICE_ID;
   if (device_id == 0x20) psbank_addr = 0xF5;
   else if (device_id == 0x2a) psbank_addr = 0x84;
   // For 'F580 and 'F960, do COBANK maintenance
   if ((device_id == 0x20) || (device_id == 0x2a))
   {
      if (addr < 0x10000L)
      {
         // PSBANK = 0x11 (default)
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         psbank_val = 0x11;
      }
      else if (addr < 0x18000L)
      {
         // PSBANK = 0x21 (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x21;
      }
      else if (addr < 0x20000L)
      {
         // PSBANK = 0x31 (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x31;
      }
      else return ADDRESS_OUT_OF_RANGE;
      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)
      C2_WriteDirect (psbank_addr, psbank_val, C2_DIRECT);
   }
   // Set up command writes
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);
   while (length != 0x0000)
   {
      // Send Block Read command
      if (return_value = C2_WriteCommand (C2_FPDAT_BLOCK_READ, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // check status
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return return_value;
      }

      // calc address translation (for devices > 64K)
      if (addr < 0x10000L)
      {
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else if (addr < 0x18000L)
      {
         // (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) (addr | 0x8000);
      }
      else if (addr < 0x20000L)
      {
         // (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else
      {
         return ADDRESS_OUT_OF_RANGE;
      }


      // now send address, high-byte first
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[MSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // Send address low byte to FPDAT
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[LSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // now set up transfer for either a 256-byte block or less
      if (length > 255)
      {
         // indicate 256-byte block
         if (return_value = C2_WriteCommand (0, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         length = length - 256;        // update length
         addr = addr + 256;            // update FLASH address
         blocksize = 256;
      }
      else
      {
         if (return_value = C2_WriteCommand (length, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         blocksize = length;
         length = 0;
      }

      // Check status
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return return_value;
      }

      // Read FLASH block
      for (i = 0; i < blocksize; i++)
      {
         if (return_value = C2_ReadData (FLASH_READ_TIMEOUT_MS))
         {
            return return_value;
         }

         *dest++ = C2_DR;
      }
   }

   // for 'F58x and 'F96x, restore PSBANK
   if ((device_id == 0x20) || (device_id == 0x2a))
   {
      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, 0x11, C2_DIRECT);

   }


   return return_value;
}


//-----------------------------------------------------------------------------
// C2_OTP_Read
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Reads a block of OTP memory of <length> starting at <addr> and
// copies it to the buffer pointed to by <dest>.
// Assumes that OTP accesses via C2 have been enabled prior to the
// function call.
//
//-----------------------------------------------------------------------------
uint8_t C2_OTP_Read (int8_t *dest, uint32_t addr, uint16_t length)
{
   uint8_t return_value;
   uint16_t i;                              // byte counter

   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }

   // set OTP read command
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_READ))
   {
      return return_value;
   }

   // send address high byte
   if (return_value = C2_WriteSFR (C2_EPADDRH, (addr >> 8)))
   {
      return return_value;
   }

   // send address low byte
   if (return_value = C2_WriteSFR (C2_EPADDRL, (uint8_t) addr))
   {
      return return_value;
   }

   // prepare for data accesses
   C2_WriteAR (C2_EPDAT);

   for (i = 0; i < length; i++)
   {
#ifdef OTP_ERROR_CHECK_ADDRESS
      // check for errors based on current EP Address
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         return return_value;
      }
      if (C2_DR & C2_EPSTAT_READ_LOCK)
      {
         return READ_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_ADDRESS

      C2_ReadAR ();                    // get Busy information

      Set_Timeout_us_1 (OTP_READ_TIMEOUT_US);
      while (C2_AR & C2_AR_OTPBUSY)    // spin until not Busy
      {
         C2_ReadAR ();
         if (Timeout_us_1)
         {
            return OTP_READ_TIMEOUT;
         }
      }

      // read the data
      if (return_value = C2_ReadDR (OTP_READ_TIMEOUT_US))
      {
         return return_value;
      }

      *dest++ = C2_DR;

#ifdef OTP_ERROR_CHECK_OPERATION
      // check for errors based on current EP operation
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         return return_value;
      }
      if (C2_DR & C2_EPSTAT_ERROR)
      {
         printf ("Read error at address 0x%05lx\n", (uint32_t) (i + addr));
//         return READ_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_OPERATION

   }
   // remove read mode sequence 1
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_WRITE1))
   {
      return return_value;
   }
   // remove read mode sequence 2
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_READ))
   {
      return return_value;
   }
   // issue device reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_RESET))
   {
      return return_value;
   }
   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }
   // return to HALT mode
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_HALT))
   {
      return return_value;
   }

   return return_value;
}


//-----------------------------------------------------------------------------
// C2_FLASH_Write
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Copies a buffer pointed to by <src> of <length> bytes to FLASH address
// starting at <addr>.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
// Note: for 128K devices, addresses cannot cross bank boundaries
//
///-----------------------------------------------------------------------------
uint8_t C2_FLASH_Write (uint32_t addr, int8_t *src, uint16_t length)
{
   uint8_t return_value;

   uint16_t i;                              // byte counter
   uint16_t blocksize;                      // size of this block transfer
   uint8_t psbank_val;
   UU16 C2_addr;
   uint8_t psbank_addr;                     // address of PSBANK SFR (Todo: move to Devices.c)
   uint8_t device_id;                       // device_id of target (for psbank maintenance)

   return_value = NO_ERROR;

   // set up "psbank_addr" for 128K devices ('F58x and 'F96x)
   device_id = KNOWN_FAMILIES[FAMILY_NUMBER].DEVICE_ID;
   if (device_id == 0x20)
   {
      psbank_addr = 0xF5;
   }
   else if (device_id == 0x2a)
   {
      psbank_addr = 0x84;
   }

   // For 'F580 and 'F960, do COBANK maintenance
   if ((device_id == 0x20) || (device_id == 0x2a))
   {

      if (addr < 0x10000L)
      {
         // PSBANK = 0x11 (default)
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         psbank_val = 0x11;
      }
      else if (addr < 0x18000L)
      {
         // PSBANK = 0x21 (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x21;
      }
      else if (addr < 0x20000L)
      {
         // PSBANK = 0x31 (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x31;
      }
      else
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, psbank_val, C2_DIRECT);

   }

   // Set up for command writes
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   while (length != 0x0000)
   {
      // Send Block Write command
      if (return_value = C2_WriteCommand (C2_FPDAT_BLOCK_WRITE, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // Check response
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return return_value;
      }

      // calc address translation (for devices > 64K)
      if (addr < 0x10000L)
      {
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else if (addr < 0x18000L)
      {
         // (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) (addr | 0x8000);
      }
      else if (addr < 0x20000L)
      {
         // (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // now send address, high-byte first
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[MSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // Send address low byte to FPDAT
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[LSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // now set up transfer for either a 256-byte block or less
      if (length > 255)
      {
         // indicate 256-byte block
         if (return_value = C2_WriteCommand (0, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         length = length - 256;        // update length
         addr = addr + 256;            // update FLASH address
         blocksize = 256;
      }
      else
      {
         if (return_value = C2_WriteCommand (length, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         blocksize = length;
         length = 0;
      }

      // Check status before writing FLASH block
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // enable VPP voltage if device is OTP
      if (KNOWN_FAMILIES[FAMILY_NUMBER].MEM_TYPE == OTP)
      {
         VPP_65_DRIVER_ON ();
         VPP_65_ENABLE ();
         Wait_us (OTP_VPP_SETTLING_TIME_US);
      }

      // Write FLASH block
      for (i = 0; i < blocksize; i++)
      {
         if (return_value = C2_WriteCommand (*src++, FLASH_WRITE_TIMEOUT_MS))
         {
            // disable VPP voltage if device is OTP
            if (KNOWN_FAMILIES[FAMILY_NUMBER].MEM_TYPE == OTP)
            {
               VPP_65_DISABLE ();
               VPP_65_DRIVER_OFF ();
            }
            return return_value;
         }
      }

      // disable VPP voltage if device is OTP
      if (KNOWN_FAMILIES[FAMILY_NUMBER].MEM_TYPE == OTP)
      {
         VPP_65_DISABLE ();
         VPP_65_DRIVER_OFF ();
      }

      // Check status
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return WRITE_ERROR;
      }
   }


   // for F58x, restore PSBANK

   if ((device_id == 0x20) || (device_id == 0x2a))
   {
      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, 0x11, C2_DIRECT);

   }

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_OTP_Write
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Copies a buffer pointed to by <src> of <length> bytes to OTP address
// starting at <addr>.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_OTP_Write (uint32_t addr, int8_t *src, uint16_t length)
{
   uint8_t return_value;
   uint16_t i;                              // byte counter

   return_value = NO_ERROR;

   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }

   // set OTP write command sequence 1
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_WRITE1))
   {
      return return_value;
   }

   // set OTP write command sequence 2
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_WRITE2))
   {
      return return_value;
   }
   // send address high byte
   if (return_value = C2_WriteSFR (C2_EPADDRH, (addr >> 8)))
   {
      return return_value;
   }

   // send address low byte
   if (return_value = C2_WriteSFR (C2_EPADDRL, (uint8_t) addr))
   {
      return return_value;
   }

   // prepare for data accesses
   C2_WriteAR (C2_EPDAT);

   VPP_65_DRIVER_ON ();
   VPP_65_ENABLE ();                   // apply programming voltage

   Wait_us (OTP_VPP_SETTLING_TIME_US);

   for (i = 0; i < length; i++)
   {

#ifdef OTP_ERROR_CHECK_ADDRESS
      // check for errors based on current EP Address
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         VPP_65_DISABLE ();            // de-assert programming voltage
         VPP_65_DRIVER_OFF ();
         return return_value;
      }
      if (C2_DR & C2_EPSTAT_WRITE_LOCK)
      {
         VPP_65_DISABLE ();            // de-assert programming voltage
         VPP_65_DRIVER_OFF ();
         return WRITE_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_ADDRESS

      // write the data
      if (return_value = C2_WriteDR (*src++, OTP_WRITE_TIMEOUT_US))
      {
         VPP_65_DISABLE ();            // de-assert programming voltage
         VPP_65_DRIVER_OFF ();
         return return_value;
      }
      C2_ReadAR ();                    // get Busy and Error information

      Set_Timeout_us_1 (OTP_WRITE_TIMEOUT_US);
      while (C2_AR & C2_AR_OTPBUSY)    // spin until not Busy
      {
         C2_ReadAR ();
         if (Timeout_us_1)
         {
            VPP_65_DISABLE ();         // de-assert programming voltage
            VPP_65_DRIVER_OFF ();
            return OTP_WRITE_TIMEOUT;
         }
      }

#ifdef OTP_ERROR_CHECK_OPERATION
      // check for errors based on current EP Operation
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         VPP_65_DISABLE ();            // de-assert programming voltage
         VPP_65_DRIVER_OFF ();
         return return_value;
      }

      if (C2_DR & C2_EPSTAT_ERROR)
      {
         VPP_65_DISABLE ();            // de-assert programming voltage
         VPP_65_DRIVER_OFF ();
         return WRITE_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_OPERATION
   }

   VPP_65_DISABLE ();                  // de-assert programming voltage
   VPP_65_DRIVER_OFF ();

   // remove write mode sequence 1
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_WRITE1))
   {
      return return_value;
   }
   // remove write mode sequence 2
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_READ))
   {
      return return_value;
   }
   // issue device reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_RESET))
   {
      return return_value;
   }
   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }
   // return to HALT mode
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_HALT))
   {
      return return_value;
   }


   return return_value;
}



//-----------------------------------------------------------------------------
// C2_FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This function erases the FLASH page containing <addr>.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_FLASH_PageErase (uint32_t addr)
{
   uint8_t return_value;
   uint8_t page_number;
   uint8_t psbank_val;
   UU16 C2_addr;
   uint8_t psbank_addr;                     // address of PSBANK SFR (Todo: move to Devices.c)
   uint8_t device_id;                       // device_id of target (for psbank maintenance)

   return_value = NO_ERROR;

   C2_addr.uint16_t = (uint16_t) addr;

   // set up "psbank_addr" for 128K devices ('F58x and 'F96x)
   device_id = KNOWN_FAMILIES[FAMILY_NUMBER].DEVICE_ID;
   if (device_id == 0x20)
   {
      psbank_addr = 0xF5;
   }
   else if (device_id == 0x2a)
   {
      psbank_addr = 0x84;
   }

   // For 'F580 and 'F960, do COBANK maintenance
   if ((device_id == 0x20) || (device_id == 0x2a))
   {

      if (addr < 0x10000L)
      {
         // PSBANK = 0x11 (default)
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         psbank_val = 0x11;

         C2_addr.uint16_t = (uint16_t) addr;
      }
      else if (addr < 0x18000L)
      {
         // PSBANK = 0x21 (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x21;
         C2_addr.uint16_t = (uint16_t) (addr | 0x8000);
      }
      else if (addr < 0x20000L)
      {
         // PSBANK = 0x31 (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         psbank_val = 0x31;
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, psbank_val, C2_DIRECT);

   }


   // Set up for command writes
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   // Send Page Erase command
   if (return_value = C2_WriteCommand (C2_FPDAT_PAGE_ERASE, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // check status
   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // calculate page number
   page_number = (uint32_t) C2_addr.uint16_t / (uint32_t) KNOWN_FAMILIES[FAMILY_NUMBER].PAGE_SIZE;

   if (page_number > (
      (uint32_t) KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_SIZE /
      (uint32_t) KNOWN_FAMILIES[FAMILY_NUMBER].PAGE_SIZE))
   {
      return ADDRESS_OUT_OF_RANGE;
   }

   // now send page number
   if (return_value = C2_WriteCommand (page_number, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // check status
   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return ADDRESS_OUT_OF_RANGE;
   }

   if (return_value = C2_WriteCommand (0x00, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // check status
   if (return_value = C2_ReadResponse (FLASH_PAGE_ERASE_TIMEOUT_MS))
   {
      return PAGE_ERASE_TIMEOUT;
   }

   // For 'F580 and 'F960, restore PSBANK
   if ((device_id == 0x20) || (device_id == 0x2a))
   {
      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, 0x11, C2_DIRECT);
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// C2_FLASH_DeviceErase
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Erases the entire FLASH memory space.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_FLASH_DeviceErase (void)
{
   uint8_t return_value;

   // Set up for commands
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   // Send Page Erase command
   if (return_value = C2_WriteCommand (C2_FPDAT_DEVICE_ERASE, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // check status
   if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now send sequence #1
   if (return_value = C2_WriteCommand (0xDE, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now send sequence #2
   if (return_value = C2_WriteCommand (0xAD, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // now send sequence #3
   if (return_value = C2_WriteCommand (0xA5, C2_POLL_INBUSY_TIMEOUT_MS))
   {
      return return_value;
   }

   // check status
   if (return_value = C2_ReadResponse (FLASH_DEVICE_ERASE_TIMEOUT_MS))
   {
      return DEVICE_ERASE_TIMEOUT;
   }

   return return_value;
}


//-----------------------------------------------------------------------------
// C2_FLASH_BlankCheck
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Reads up to the entire FLASH memory, confirming that each byte is 0xFF.
// Returns NO_ERROR if the device is blank, DEVICE_NOT_BLANK or
// DEVICE_READ_PROTECTED otherwise.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_FLASH_BlankCheck (uint32_t addr, uint32_t length)
{
   uint8_t return_value;

   uint16_t i;                              // byte counter
   uint32_t blocksize;                      // size of this block transfer
   static bit device_is_blank;
   uint8_t psbank_val;
   UU16 C2_addr;
   uint8_t idata psbank_addr;               // address of PSBANK SFR (Todo: move to Devices.c)
   uint8_t idata device_id;                 // device_id of target (for psbank maintenance)

   device_is_blank = TRUE;

   return_value = NO_ERROR;

   // set up "psbank_addr" for 128K devices ('F58x and 'F96x)
   device_id = KNOWN_FAMILIES[FAMILY_NUMBER].DEVICE_ID;
   if (device_id == 0x20)
   {
      psbank_addr = 0xF5;
   }
   else if (device_id == 0x2a)
   {
      psbank_addr = 0x84;
   }


   // Set up for command writes
   C2_WriteAR (KNOWN_FAMILIES[FAMILY_NUMBER].FPDAT);

   while (length != 0x0000L)
   {

      // For 'F580 and 'F960, do COBANK maintenance
      if ((device_id == 0x20) || (device_id == 0x2a))
      {

         if (addr < 0x10000L)
         {
            // PSBANK = 0x11 (default)
            // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
            psbank_val = 0x11;
         }
         else if (addr < 0x18000L)
         {
            // PSBANK = 0x21 (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
            psbank_val = 0x21;
         }
         else if (addr < 0x20000L)
         {
            // PSBANK = 0x31 (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
            psbank_val = 0x31;
         }
         else
         {
            return ADDRESS_OUT_OF_RANGE;
         }

         // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

         C2_WriteDirect (psbank_addr, psbank_val, C2_DIRECT);

      }
   
      // Send Block Read command
      if (return_value = C2_WriteCommand (C2_FPDAT_BLOCK_READ, C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // check status
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return return_value;
      }

      // calc address translation (for devices > 64K)
      if (addr < 0x10000L)
      {
         // linear addresses 0x00000 to 0x0FFFF map as follows: 0x0000 to 0xFFFF
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else if (addr < 0x18000L)
      {
         // (0x10000 to 0x17FFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) (addr | 0x8000);
      }
      else if (addr < 0x20000L)
      {
         // (0x18000 to 0x1FFFF map to 0x8000 to 0xFFFF)
         C2_addr.uint16_t = (uint16_t) addr;
      }
      else
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // now send address, high-byte first
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[MSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // Send address low byte to FPDAT
      if (return_value = C2_WriteCommand (C2_addr.uint8_t[LSB], C2_POLL_INBUSY_TIMEOUT_MS))
      {
         return return_value;
      }

      // now set up transfer for either a 256-byte block or less      
      if (length > 255)
      {
         // indicate 256-byte block
         if (return_value = C2_WriteCommand (0, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         length = length - 256;        // update length
         addr = addr + 256;            // update FLASH address
         blocksize = 256;
      }
      else
      {
         if (return_value = C2_WriteCommand (length, C2_POLL_INBUSY_TIMEOUT_MS))
         {
            return return_value;
         }
         blocksize = length;
         length = 0;
      }

      // Check status before reading FLASH block
      if (return_value = C2_ReadResponse (C2_POLL_OUTREADY_TIMEOUT_MS))
      {
         return ADDRESS_OUT_OF_RANGE;
      }

      // Read FLASH block
      for (i = 0; i < blocksize; i++)
      {
         if (return_value = C2_ReadData (FLASH_READ_TIMEOUT_MS))
         {
            return return_value;
         }

         if (C2_DR != 0xFF)
         {
            device_is_blank = FALSE;
         }
      }
   }

   if (device_is_blank == FALSE)
   {
      return_value = DEVICE_NOT_BLANK;
   }
   else
   {
      return_value = DEVICE_IS_BLANK;
   }

   // For 'F580 and 'F960, restore PSBANK
   if ((device_id == 0x20) || (device_id == 0x2a))
   {
      // update PSBANK accordingly (need to push PSBANK SFR address into Device.c structure)

      C2_WriteDirect (psbank_addr, 0x11, C2_DIRECT);

   }


   return return_value;
}



//-----------------------------------------------------------------------------
// C2_OTP_BlankCheck
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// Reads up to the entire OTP memory, confirming that each byte is 0xFF.
// Returns NO_ERROR if the device is blank, DEVICE_NOT_BLANK or
// DEVICE_READ_PROTECTED otherwise.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_OTP_BlankCheck (uint32_t addr, uint32_t length)
{
   uint8_t return_value;
   uint16_t i;                              // byte counter
   static bit device_is_blank;

   device_is_blank = TRUE;

   return_value = NO_ERROR;

   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }

   // set OTP read command
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_READ))
   {
      return return_value;
   }

   // send address high byte
   if (return_value = C2_WriteSFR (C2_EPADDRH, (addr >> 8)))
   {
      return return_value;
   }

   // send address low byte
   if (return_value = C2_WriteSFR (C2_EPADDRL, (uint8_t) addr))
   {
      return return_value;
   }

   // prepare for data accesses
   C2_WriteAR (C2_EPDAT);

   for (i = 0; i < length; i++)
   {
#ifdef OTP_ERROR_CHECK_ADDRESS
      // check for errors based on current EP Address
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         return return_value;
      }
      if (C2_DR & C2_EPSTAT_READ_LOCK)
      {
         return READ_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_ADDRESS

      C2_ReadAR ();                    // get Busy and Error information

      Set_Timeout_ms_1 (OTP_READ_TIMEOUT_US);
      while (C2_AR & C2_AR_OTPBUSY)    // spin until not Busy
      {
         C2_ReadAR ();
         if (Timeout_ms_1)
         {
            return OTP_READ_TIMEOUT;
         }
      }

      // read the data
      if (return_value = C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         return return_value;
      }

      if (C2_DR != 0xFF)
      {
//         return_value = DEVICE_NOT_BLANK;
//         length = 0x0000L;
//         break;
         device_is_blank = FALSE;
      }

#ifdef OTP_ERROR_CHECK_OPERATION
      // check for errors based on current EP operation
      C2_WriteAR (C2_EPSTAT);
      if (C2_ReadDR (C2_READ_DR_TIMEOUT_US))
      {
         return return_value;
      }
      if (C2_DR & C2_EPSTAT_ERROR)
      {
         return READ_ERROR;
      }

      // prepare for data accesses
      C2_WriteAR (C2_EPDAT);
#endif // OTP_ERROR_CHECK_OPERATION

   }
   // remove read mode sequence 1
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_WRITE1))
   {
      return return_value;
   }
   // remove read mode sequence 2
   if (return_value = C2_WriteSFR (C2_EPCTL, C2_EPCTL_READ))
   {
      return return_value;
   }
   // issue device reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_RESET))
   {
      return return_value;
   }
   // issue core reset
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_CORE_RESET))
   {
      return return_value;
   }
   // return to HALT mode
   if (return_value = C2_WriteSFR (C2_FPCTL, C2_FPCTL_HALT))
   {
      return return_value;
   }

   if (device_is_blank == FALSE)
   {
      return_value = DEVICE_NOT_BLANK;
   }
   else
   {
      return_value = DEVICE_IS_BLANK;
   }

   return return_value;
}

/*
//-----------------------------------------------------------------------------
// C2_Get_LockByte
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This function returns the value of the LockByte.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
uint8_t C2_Get_LockByte (void)
{
   uint8_t return_value;
   return return_value;
}

*/
//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

