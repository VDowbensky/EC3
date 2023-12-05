//-----------------------------------------------------------------------------
// Device_Programmer_Main.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// This program implements a C2 device programmer.
// How To Test:
//
// 1) Connect terminal program.  Initialize baud rate and 8-N-1 and hardware
//    RTS and CTS handshaking.
// 2) Run the program and observe the terminal output.
//
// Target:         C8051F38x
// Tool chain:     Keil C51 9.51
// Command Line:   None
//
// Release 1.2 (JL)
//    - Supported more MCU families.
//    - 15 JAN 2015
// Release 1.1 (BW)
//    - ported to 'F340 before 8/17/10
// Release 1.0
//    -Initial Revision (BW)
//    -11 MAR 2008
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Common.h"
#include "Init.h"
#include "Timing.h"
#include "Pins.h"
#include "C2_Utils.h"
#include "C2_Flash.h"
#include "HEX_Utils.h"
#include "Errors.h"
#include "Serial.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

// UART-controlled command menu
typedef struct
{
   U8 number;                          // unique command number
   S8 *name;                           // unique command identifier string
   U8 name_size;                       // size of name above
   S8 *usage;                          // usage string
   S8 *menu;                           // menu string
   U8 display;                         // flag to display command or not
} COMMAND;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

//void main (void);

U8 Display_Menu (void);
U8 Menu (void);
U8 Command_Decode (S8 * instr);

U8 OP_Write_TARGET2HEX (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// Command table
SEGMENT_VARIABLE (Command_Table[], COMMAND, SEG_CODE) =
{
   { 0, "da              ",  2, "da                       ", "Device Autodetect", TRUE},
   { 1, "?               ",  1, "?                        ", "Print Menu", TRUE},
   { 2, "wms             ",  3, "wms <16d>                ", "Wait ms", FALSE},
   { 3, "wus             ",  3, "wus <16d>                ", "Wait us", FALSE},
   { 4, "start           ",  5, "start                    ", "Start Stopwatch", FALSE},
   { 5, "stop            ",  4, "stop                     ", "Stop Stopwatch", FALSE},
   { 6, "stoms           ",  5, "stoms <16d>              ", "Set Timeout ms", FALSE},
   { 7, "stous           ",  5, "stous <16d>              ", "Set Timeout us", FALSE},
   { 8, "pi              ",  2, "pi                       ", "Pin init", TRUE},
   { 9, "c2rst           ",  5, "c2rst                    ", "C2 Reset", TRUE},
   {10, "wa              ",  2, "wa <8h>                  ", "C2 Write Address", TRUE},
   {11, "ra              ",  2, "ra                       ", "C2 Read Address", TRUE},
   {12, "wd              ",  2, "wd <8h>                  ", "C2 Write Data", TRUE},
   {13, "rd              ",  2, "rd                       ", "C2 Read Data", TRUE},
   {14, "c2halt          ",  6, "c2halt                   ", "C2 Reset and Halt", TRUE},
   {15, "c2dev           ",  5, "c2dev                    ", "C2 Get Device ID", TRUE},
   {16, "c2rev           ",  5, "c2rev                    ", "C2 Get Revision ID", TRUE},
   {17, "c2rsfr          ",  6, "c2rsfr <8h>              ", "C2 Read SFR", TRUE},
   {18, "c2wsfr          ",  6, "c2wsfr <8h> <8h>         ", "C2 Write SFR", TRUE},
   {19, "c2rd            ",  4, "c2rd <8h>                ", "C2 Read Direct", TRUE},
   {20, "c2wd            ",  4, "c2wd <8h> <8h>           ", "C2 Write Direct", TRUE},
   {21, "c2ri            ",  4, "c2ri <8h>                ", "C2 Read Indirect", TRUE},
   {22, "c2wi            ",  4, "c2wi <8h> <8h>           ", "C2 Write Indirect", TRUE},
   {23, "c2d             ",  3, "c2d                      ", "C2 Discover", TRUE},
   {24, "init            ",  4, "init                     ", "Run Init String", TRUE},
   {25, "c2fr            ",  4, "c2fr <32h> <16d>         ", "C2 Flash Read <start addr> <length>", TRUE},
   {26, "c2or            ",  4, "c2or <32h> <16d>         ", "C2 OTP Read <start addr> <length>", TRUE},
   {27, "c2fw            ",  4, "c2fw <32h> <strhex>      ", "C2 Flash Write <start addr> <length> <hex string>", TRUE},
   {28, "c2ow            ",  4, "c2ow <32h> <strhex>      ", "C2 OTP Write <start addr> <length> <hex string>", TRUE},
   {29, "c2fpe           ",  5, "c2fpe <32h>              ", "C2 Page Erase <address in page to erase>", TRUE},
   {30, "c2fde           ",  5, "c2fde                    ", "C2 Device Erase", TRUE},
   {31, "c2fbc           ",  5, "c2fbc                    ", "C2 Flash Blank Check", TRUE},
   {32, "c2obc           ",  5, "c2obc                    ", "C2 OTP Blank Check", TRUE},
   {33, "c2glb           ",  5, "c2glb                    ", "C2 Get Lock Byte value", TRUE},
   {34, "wt2h            ",  4, "wt2h                     ", "Write Target to HEX", TRUE},
   {35, "rsfrs           ",  5, "rsfrs                    ", "Read SFRs", TRUE},
   {36, "c2step           ",  6, "c2step                    ", "C2 Step", TRUE},
   {37, "c2crc            ", 5, "c2crc <8h>               ", "C2 CRC Block <256 byte block #>", TRUE},
   {00, NULL,                0, NULL, NULL, FALSE},
   {99, "",                  0, "Error                         ", "", TRUE}
};

static SEGMENT_VARIABLE (dest[16], S8, SEG_PDATA);
static SEGMENT_VARIABLE (hexdest[33], S8, SEG_PDATA);
// input string in pdata space
static SEGMENT_VARIABLE (In_Str[64], S8, SEG_PDATA);

//static U8 ** SEG_PDATA COMMAND_LIST;
static SEGMENT_VARIABLE_SEGMENT_POINTER (COMMAND_LIST, U8 *, , SEG_PDATA);

static SEGMENT_VARIABLE (HEX_String[80], U8, SEG_XDATA);
static SEGMENT_VARIABLE (Binary_Buf[32], U8, SEG_XDATA);
static SEGMENT_VARIABLE (My_HEX, HEX_RECORD, SEG_XDATA);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   WDT_Init ();
   PORT_Init ();
   SYSCLK_Init ();
   UART0_Init ();
   UART_RX_FIFO_Init ();
   Timer0_Init ();
   PCA0_Init ();

   // initialize "connection" variables
   //FAMILY_FOUND = FALSE;
   //DERIVATIVE_FOUND = FALSE;
   DEVICE_HALTED = FALSE;

   UART_RX_FIFO_Enable ();

   ENABLE_INTERRUPTS ();               // enable global variables

   Display_Menu ();                    // display menu and prompt

   while (1)
   {
      COMMAND_LIST = (U8 **) NULL;
      Menu ();

      if (COMMAND_LIST != (U8 **) NULL)
      {
         while (*COMMAND_LIST != NULL)
         {
            Command_Decode (*COMMAND_LIST);
            COMMAND_LIST++;
         }
      }

   }
}

//-----------------------------------------------------------------------------
// Menu Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Display_Menu
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Prints the command menu to stdio.
//
//-----------------------------------------------------------------------------
U8 Display_Menu (void)
{
   COMMAND *ctptr;                     // command table pointer

   ctptr = (COMMAND *) Command_Table;

   printf ("\n8-bit Device Programmer ver %s\n", VERSION);
   printf (" #  Command\t\tUsage         \t\t\tDescription\n");
   while (ctptr->name != NULL)
   {
      if (ctptr->display == TRUE)
      {
         printf ("%02u: ", (U16) ctptr->number);
         printf ("%s", ctptr->name);
         printf ("\t%s", ctptr->usage);
         printf ("\t%s", ctptr->menu);
         printf ("\n");
      }
      ctptr++;
   }

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Menu
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Gets a string from stdio, parses it, and calls the appropriate functions.
//
//-----------------------------------------------------------------------------
U8 Menu (void)
{
   U8 return_value = NO_ERROR;

   printf ("> ");

   GETS (In_Str, sizeof (In_Str)-1);
   printf ("\n");

   if (strlen (In_Str) > 0)
   {
      return_value = Command_Decode (In_Str);
   }

   return return_value;
}

//-----------------------------------------------------------------------------
// Command_Decode
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : Pointer to input string
//
// Parses the command string and calls the appropriate functions.
//
//-----------------------------------------------------------------------------
U8 Command_Decode (S8 * instr)
{
   U8 return_value;
   S8 *cp;                             // character pointer
   U8 command_number;                  // number of command encoded in 'instr'
   COMMAND *ctptr;                     // Command_Table pointer
   DEVICE_FAMILY *dfptr;               // pointer to current device family
   DERIVATIVE_ENTRY *deptr;            // pointer to current derivative
   U8 deviceid;
   U8 derivativeid;

   command_number = 99;                // initialize command_number to an
                                       // invalid selection
   deviceid = 0xffff;                  // initialize device and derivative
   derivativeid = 0xffff;              // id's to invalid selections

   return_value = INVALID_COMMAND;     // initialize return error code to
                                       // an invalid selection
   printf ("Command: %s\n", instr);
   if ((instr[0] >= '0') && instr[0] <= '9')
   {
      U16 temp;                        // if first char is a digit, then
      temp = atoi (instr);             // interpret it as a command number
      command_number = temp;
   }
   else
   {
      // interpret command as a string and find command number
      ctptr = Command_Table;

      while (ctptr->name != NULL)
      {
         if (strncmp (instr, ctptr->name, ctptr->name_size) == 0)
         {
            // we've found the command, so record its number and exit
            command_number = ctptr->number;
            break;
         }
         ctptr++;
      }

      if (ctptr->name == NULL)
      {
         command_number = 99;             // indicate invalid command
         return_value = INVALID_COMMAND;
      }
   }

   // Now we have a command number, so act on it.
   switch (command_number)
   {
      case 0:
      {
         printf ("Device Autodetect\n");
         Start_Stopwatch ();
         return_value = Pin_Init ();
         if (return_value == NO_ERROR)
         {
            return_value = C2_Halt ();
            if (return_value == NO_ERROR)
            {
               return_value = C2_Discover (&deviceid, &derivativeid);
               if (return_value == NO_ERROR)
               {
                  COMMAND_LIST = (U8 **) KNOWN_FAMILIES[FAMILY_NUMBER].INIT_STRINGS;
               }
            }
         }
         Stop_Stopwatch ();
         printf ("Device ID returned 0x%04x\n", (U16) deviceid);
         printf ("Derivative ID returned 0x%04x\n", (U16) derivativeid);
         break;
      }
      case 1:
      {
         printf ("? stub\n");
         Start_Stopwatch ();
         Display_Menu ();
         Stop_Stopwatch ();
         return_value = NO_ERROR;
         break;
      }
      case 2:
      {
         U16 wait_time;

         cp = instr;
         while (*cp++ != ' ');

         wait_time = atoi (cp);

         printf ("Waiting %d ms\n", wait_time);
         Start_Stopwatch ();
         Wait_ms (wait_time);
         Stop_Stopwatch ();
         printf ("Stopwatch_ms is %u\n", Stopwatch_ms);

         return_value = NO_ERROR;
         break;
      }
      case 3:
      {
         U16 wait_time;

         cp = instr;
         while (*cp++ != ' ');

         wait_time = atoi (cp);

         printf ("Waiting %d us\n", wait_time);
         Start_Stopwatch ();
         Wait_us (wait_time);
         Stop_Stopwatch ();
         printf ("Stopwatch_us is %u\n", Stopwatch_us);

         return_value = NO_ERROR;
         break;
      }
      case 4:
      {
         printf ("Start Stopwatch\n");
         return_value = Start_Stopwatch ();
         break;
      }
      case 5:
      {
         printf ("Stop Stopwatch\n");
         return_value = Stop_Stopwatch ();
         printf ("Stopwatch_ms is %u\n", Stopwatch_ms);
         printf ("Stopwatch_us is %u\n", Stopwatch_us);
         break;
      }
      case 6:
      {
         U16 wait_time;

         printf ("Set Timeout ms:\n");

         cp = instr;
         while (*cp++ != ' ');

         wait_time = atoi (cp);

         printf ("Timing out for %d ms\n", wait_time);
         Start_Stopwatch ();
         Set_Timeout_ms_1 (wait_time);
         while (!Timeout_ms_1);
         Stop_Stopwatch ();
         printf ("Stopwatch_ms is %u\n", Stopwatch_ms);

         return_value = NO_ERROR;
         break;
      }
      case 7:
       {
         U16 wait_time;

         printf ("Set Timeout us\n");

         cp = instr;
         while (*cp++ != ' ');

         wait_time = atoi (cp);

         printf ("Timing out for %d us\n", wait_time);
         Start_Stopwatch ();
         Set_Timeout_us_1 (wait_time);
         while (!Timeout_us_1);
         Stop_Stopwatch ();
         printf ("Stopwatch_us is %u\n", Stopwatch_us);

         return_value = NO_ERROR;
         break;
      }
      case 8:
      {
         printf ("Pin Init\n");
         return_value = Pin_Init ();

         break;
      }
      case 9:
      {
         printf ("C2 Reset\n");
         return_value = C2_Reset ();

         break;
      }
      case 10:
      {
         U16 addr;

         printf ("C2 Write Address\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = atox (cp);
         printf ("Writing address 0x%02x\n", addr);
         Start_Stopwatch ();
         return_value = C2_WriteAR ((U8) addr);
         Stop_Stopwatch ();
         break;
      }
      case 11:
      {
         printf ("C2 Read Address\n");
         Start_Stopwatch ();
         return_value = C2_ReadAR ();
         Stop_Stopwatch ();
         printf ("Address returned is 0x%02x\n", (U16) C2_AR);

         break;
      }
      case 12:
      {
         U8 thedata;

         printf ("C2 Write Data\n");

         cp = instr;
         while (*cp++ != ' ');

         thedata = (U8) atox (cp);

         printf ("Writing %u\n", (U16) thedata);
         Start_Stopwatch ();
         return_value = C2_WriteDR (thedata, C2_WRITE_DR_TIMEOUT_US);
         Stop_Stopwatch ();

         break;
      }
      case 13:
      {
         U8 thedata;

         printf ("C2 Read Data\n");

         Start_Stopwatch ();
         return_value = C2_ReadDR (C2_READ_DR_TIMEOUT_US);
         Stop_Stopwatch ();
         thedata = C2_DR;
         printf ("Data register is 0x%02x\n", (U16) thedata);

         break;
      }
      case 14:
      {
         printf ("C2 Reset and Halt\n");
         return_value = C2_Halt ();

         break;
      }
      case 15:
      {
         U8 devid;

         printf ("C2 Get Device ID\n");
         Start_Stopwatch ();
         return_value = C2_GetDevID (&devid);
         Stop_Stopwatch ();
         printf ("Device ID is %u, 0x%04x\n", (U16) devid, (U16) devid);

         break;
      }
      case 16:
      {
         U8 revid;

         printf ("C2 Get Revision ID\n");
         Start_Stopwatch ();
         return_value = C2_GetRevID (&revid);
         Stop_Stopwatch ();
         printf ("Revision ID is %u, 0x%04x\n", (U16) revid, (U16) revid);

         break;
      }
      case 17:
      {
         U8 sfr_value;
         U8 sfr_address;

         printf ("C2 Read SFR\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         printf ("Reading SFR 0x%02x\n", (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_ReadSFR (sfr_address, &sfr_value);
         Stop_Stopwatch ();
         printf ("Read SFR returned 0x%02x\n", (U16) sfr_value);

         break;
      }
      case 18:
      {
         U8 sfr_address;
         U8 sfr_value;

         printf ("C2 Write SFR\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         while (*cp++ != ' ');

         sfr_value = (U8) atox (cp);

         printf ("Writing 0x%02x to address 0x%02x\n", (U16) sfr_value,
                (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_WriteSFR (sfr_address, sfr_value);
         Stop_Stopwatch ();

         break;
      }
      case 19:
      {
         U8 sfr_value;
         U8 sfr_address;

         printf ("C2 Read Direct\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         printf ("Reading Direct 0x%02x\n", (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_ReadDirect (sfr_address, &sfr_value, C2_DIRECT);
         Stop_Stopwatch ();
         printf ("Read Direct returned 0x%02x\n", (U16) sfr_value);

         break;
      }
      case 20:
      {
         U8 sfr_address;
         U8 sfr_value;

         printf ("C2 Write Direct\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         while (*cp++ != ' ');

         sfr_value = (U8) atox (cp);

         printf ("Writing 0x%02x to address 0x%02x\n", (U16) sfr_value,
                (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_WriteDirect (sfr_address, sfr_value, C2_DIRECT);
         Stop_Stopwatch ();

         break;
      }
      case 21:
      {
         U8 sfr_value;
         U8 sfr_address;

         printf ("C2 Read Indirect\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         printf ("Reading Indirect 0x%02x\n", (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_ReadDirect (sfr_address, &sfr_value, C2_INDIRECT);
         Stop_Stopwatch ();
         printf ("Read Indirect returned 0x%02x\n", (U16) sfr_value);

         break;
      }
      case 22:
      {
         U8 sfr_address;
         U8 sfr_value;

         printf ("C2 Write Indirect\n");

         cp = instr;
         while (*cp++ != ' ');

         sfr_address = (U8) atox (cp);

         while (*cp++ != ' ');

         sfr_value = (U8) atox (cp);

         printf ("Writing 0x%02x to address 0x%02x\n", (U16) sfr_value,
                (U16) sfr_address);
         Start_Stopwatch ();
         return_value = C2_WriteDirect (sfr_address, sfr_value, C2_INDIRECT);
         Stop_Stopwatch ();

         break;
      }
      case 23:
      {
         U8 j;

         printf ("C2 Discover\n");
         Start_Stopwatch ();
         return_value = C2_Discover (&deviceid, &derivativeid);
         Stop_Stopwatch ();

         if (return_value != NO_ERROR)
         {
            break;
         }
         dfptr = &(KNOWN_FAMILIES[FAMILY_NUMBER]);
         deptr = &(KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER]);

         printf ("Family Information:\n");
         printf ("Device ID: 0x%04x\n", dfptr->DEVICE_ID);
         printf ("Device rev: 0x%04x\n", (U16) DEVICE_REV);
         printf ("Family string: %s\n", dfptr->FAMILY_STRING);
         printf ("Mem Type: %u\n", (U16) dfptr->MEM_TYPE);
         printf ("Page Size: %u\n", dfptr->PAGE_SIZE);
         printf ("Has SFLE: %u\n", (U16) dfptr->HAS_SFLE);
         printf ("Security Type: %u\n", (U16) dfptr->SECURITY_TYPE);
         printf ("FPDAT address: 0x%02x\n", (U16) dfptr->FPDAT);
         printf ("Init strings:\n");
         for (j = 0; ; j++)
         {
            if (dfptr->INIT_STRINGS[j] == NULL)
            {
               break;
            }
            printf ("%s\n", dfptr->INIT_STRINGS[j]);
         }
         printf ("\n");
         printf ("Derivative Information:\n");
         printf ("Derivative ID: 0x%04x\n", deptr->DERIVATIVE_ID);
         printf ("Derivative rev: 0x%04x\n", (U16) DERIVATIVE_REV);
         printf ("Derivative String: %s\n", deptr->DERIVATIVE_STRING);
         printf ("Features String: %s\n", deptr->FEATURES_STRING);
         printf ("Package String: %s \n", deptr->PACKAGE_STRING);
         printf ("Code Start Address: 0x%05lx\n", deptr->CODE_START);
         printf ("Code Size: 0x%05lx\n", deptr->CODE_SIZE);
         printf ("Write Lock Byte Addr: 0x%05lx\n", deptr->WRITELOCKBYTEADDR);
         printf ("Read Lock Byte Addr: 0x%05lx\n", deptr->READLOCKBYTEADDR);
         printf ("Code 2 Start Address: 0x%05lx\n", deptr->CODE2_START);
         printf ("Code 2 Size: 0x%05lx\n", deptr->CODE2_SIZE);
         printf ("\n");

         break;
      }
      case 24:
      {
         return_value = NO_ERROR;
         printf ("Execute Device Init String:\n");
         if (FAMILY_FOUND == TRUE)
         {
            COMMAND_LIST = (U8 **) KNOWN_FAMILIES[FAMILY_NUMBER].INIT_STRINGS;
         }
         else
         {
            printf ("Device not connected.\n");
         }
         break;
      }
      case 25:
      {
         U32 addr;
         U16 length;

         printf ("C2 Flash Read\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = (U32) atolx (cp);

         while (*cp++ != ' ');

         length = (U16) atoi (cp);

         if (length > sizeof (dest))
         {
            length = sizeof (dest);
         }

         printf ("Reading %u bytes starting at address 0x%05lx\n",
                 length, addr);
         Start_Stopwatch ();
         return_value = C2_FLASH_Read (dest, addr, length);
         Stop_Stopwatch ();

         BIN2HEXSTR (hexdest, dest, length);
         printf ("Memory contents are %s\n", hexdest);

         break;
      }
      case 26:
      {
         U32 addr;
         U16 length;

         printf ("C2 OTP Read\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = (U32) atolx (cp);

         while (*cp++ != ' ');

         length = (U16) atoi (cp);

         if (length > sizeof (dest))
         {
            length = sizeof (dest);
         }

         printf ("Reading %u bytes starting at address 0x%05lx\n",
                 length, addr);
         Start_Stopwatch ();
         return_value = C2_OTP_Read (dest, addr, length);
         Stop_Stopwatch ();

         BIN2HEXSTR (hexdest, dest, length);
         printf ("Memory contents are %s\n", hexdest);
         break;
      }
      case 27:
      {
         U32 addr;
         U8 length;

         printf ("C2 Flash Write\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = (U32) atolx (cp);

         while (*cp++ != ' ');

         // warning! 'dest' could be overtaken by a long string
         HEXSTR2BIN (dest, cp, &length);

         if (length > sizeof (dest))
         {
            length = sizeof (dest);
         }

         printf ("Writing %u bytes starting at address 0x%05lx\n",
                 (U16) length, addr);
         printf ("Writing the following string: %s\n", cp);
         Start_Stopwatch ();
         return_value = C2_FLASH_Write (addr, dest, length);
         Stop_Stopwatch ();
         break;
      }
      case 28:
      {
         U32 addr;
         U8 length;

         printf ("C2 OTP Write\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = (U32) atolx (cp);

         while (*cp++ != ' ');

         // warning! 'dest' could be overtaken by a long string
         HEXSTR2BIN (dest, cp, &length);

         if (length > sizeof (dest))
         {
            length = sizeof (dest);
         }

         printf ("Writing %u bytes starting at address 0x%05lx\n",
                 (U16) length, addr);
         printf ("Writing the following string: %s\n", cp);
         Start_Stopwatch ();
         return_value = C2_OTP_Write (addr, dest, length);
         Stop_Stopwatch ();
         break;
      }
      case 29:
      {
         U32 addr;

         printf ("C2 Flash Page Erase\n");

         cp = instr;
         while (*cp++ != ' ');

         addr = (U32) atolx (cp);

         printf ("Erasing page containing address 0x%05lx\n", addr);
         Start_Stopwatch ();
         return_value = C2_FLASH_PageErase (addr);
         Stop_Stopwatch ();

         break;
      }
      case 30:
      {
         printf ("C2 Flash Device Erase\n");

         printf ("Erasing device...\n");
         Start_Stopwatch ();
         return_value = C2_FLASH_DeviceErase ();
         Stop_Stopwatch ();

         break;
      }
      case 31:
      {
         U32 addr;
         U32 length;

         printf ("C2 Flash Blank Check\n");

         addr = KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_START;
         length = KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_SIZE;

         printf ("Checking starting at address 0x%05lx for 0x%05lx bytes:\n", addr, length);

         Start_Stopwatch ();
         return_value = C2_FLASH_BlankCheck (addr, length);
         Stop_Stopwatch ();

         break;
      }
      case 32:
      {
         U32 addr;
         U32 length;

         printf ("C2 OTP Blank Check\n");

         addr = KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_START;
         length = KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_SIZE;

         printf ("Checking starting at address 0x%05lx for 0x%05lx bytes:\n", addr, length);

         Start_Stopwatch ();
         return_value = C2_OTP_BlankCheck (addr, length);
         Stop_Stopwatch ();

         break;
      }
      case 33:
      {
         printf ("C2 Get Lock Byte\n");
         break;
      }
      case 34:
      {
         printf ("Write Target to HEX:\n");
         Start_Stopwatch ();
         return_value = OP_Write_TARGET2HEX ();
         Stop_Stopwatch ();

         break;
      }
      case 35:                         // Read SFRs and directs
      {
         U8 row;
         U8 col;
         U8 value;

         row = 0xf8;
         col = 0x00;
         Start_Stopwatch ();
         for (row = 0xf8; row != 0x00; row = row - 8)
         {
            for (col = 0; col != 0x08; col++)
            {
               if (return_value = C2_ReadDirect ((row+col), &value, C2_DIRECT))
               {
                  break;
               }
               if (col == 0)
               {
                  printf ("\n0x%02x: %02x", (U16) (row), (U16) value);
               }
               else
               {
                  printf (" %02x", (U16) value);
               }
            } // for col
         } // for row
         printf ("\n\n");
         Stop_Stopwatch ();
         break;
      }
      case 36:
      {
        Start_Stopwatch ();
        return_value = C2_Step();
        Stop_Stopwatch ();
        break;
      }
      case 37:
      {
    	  U8 block;
    	  U16 address;
          U16 crc;

          printf ("C2 Flash Page Erase\n");

          cp = instr;
          while (*cp++ != ' ');

          block = (U8) atolx (cp);

          address = block * 256;

          printf ("CRC'ing 256-byte block of flash starting at address 0x%04x\n", address);
          Start_Stopwatch ();
          return_value = C2_GetPageCRC(block, &crc);
          Stop_Stopwatch ();
          printf ("CRC: 0x%04x\n", crc);

          break;
      }
      default:
      {
         printf ("Invalid command\n");
         return_value = INVALID_COMMAND;
      }
   }

   if (Stopwatch_ms < 64)
   {
      printf ("Operation took %u us\n", Stopwatch_us);
   }
   else
   {
      printf ("Operation took %lu ms\n", Stopwatch_ms);
   }

   printf ("Returned error code %u: '%s'\n", (U16) return_value,
            ERROR_List[return_value].Error_string);

   LED1_State = LED_ON;

   return return_value;
}


//-----------------------------------------------------------------------------
// OP_Write_TARGET2HEX
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : family_number and derivative number
//
// This function reads the entire code contents of a device and outputs
// HEX records to the UART.
//
// To do: -- add SFLE support
//        -- add support for variable starting address and length
//
//-----------------------------------------------------------------------------
U8 OP_Write_TARGET2HEX (void)
{
   U8 return_value;
   U8 reclen;
   U8 i;
   bit blank;
   U32 length;
   U32 address;
   U8 mem_type;

   length = KNOWN_FAMILIES[FAMILY_NUMBER].DERIVATIVE_LIST[DERIVATIVE_NUMBER].CODE_SIZE;
   address = 0x0L;
   mem_type = KNOWN_FAMILIES[FAMILY_NUMBER].MEM_TYPE;

   return_value = NO_ERROR;

   LED1_State = LED_UART_RX_TX;

   while (length != 0x0L)
   {
      // calculate size of hex record
      if (length > HEX_RECLEN)
      {
         reclen = HEX_RECLEN;
         length = length - reclen;
      }
      else
      {
         reclen = length;
         length = 0;
      }

      if (mem_type == FLASH)
      {
         // read the target buffer
         if (return_value = C2_FLASH_Read (Binary_Buf, address, reclen))
         {
            return return_value;
         }
      }
      else if (mem_type == OTP)
      {
         // read the target buffer
         if (return_value = C2_OTP_Read (Binary_Buf, address, reclen))
         {
            return return_value;
         }
      }

      // at this point, ibuf contains the contents of the desired HEX record.
      // Populate the record.
      My_HEX.Buf = Binary_Buf;
      My_HEX.RECLEN = reclen;
      My_HEX.RECTYP = HEXREC_DAT;
      My_HEX.OFFSET.U16 = address;

      // update address
      address = address + reclen;

      // Suppress 0xFF records

      // check for 0xFF's
      blank = TRUE;
      for (i = 0; i < reclen; i++)
      {
         if (Binary_Buf[i] != 0xff)
         {
            blank = FALSE;
            break;
         }
      }

      if (blank == FALSE)
      {
         HEX_Encode (HEX_String, &My_HEX, 1);

         printf ("%s\n", HEX_String);
      }
   }

   // print ending record
   printf (":00000001FF\n");

   return return_value;
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

