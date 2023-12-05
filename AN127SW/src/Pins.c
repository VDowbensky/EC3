//-----------------------------------------------------------------------------
// Pins.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for initializing the C2 and JTAG pin
// connections.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Common.h"
#include "Timing.h"
#include "Pins.h"
#include "Errors.h"

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

U8 Pin_Init (void);
U8 Pin_Test (void);

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Pin_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure port pins used for C2 and JTAG interfaces.
//
// The pinout is optimized to map the standard Port I/O header to the
// standard 10-pin debug connector, as follows:
// Pin #   C2 debug   JTAG debug   F380 pin     F380 configuration
// Pin 1   +3V        +3V          P3.0         OD high
// Pin 2   GND        GND          P3.1         PP low -- simulate ground
// Pin 3   GND        GND          P3.2         PP low -- simulate ground
// Pin 4   C2D        TCK          P3.3         OD high
// Pin 5   /RST_share TMS          P3.4         OD high
// Pin 6   C2D_share  TDO          P3.5         OD high
// Pin 7   C2CK       TDI          P3.6         OD high
// Pin 8   VPP_65     VPP_65       P3.7         OD high
// Pin 9   GND        GND          +3.3V        ** MUST CUT stake header! **
// Pin 10  VBUS       NC           GND          no connect
//
//-----------------------------------------------------------------------------

U8 Pin_Init (void)
{
   P3 = 0xF9;                          // init P3 state
   P3MDOUT = 0x06;                     //  and direction

   return NO_ERROR;
}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

