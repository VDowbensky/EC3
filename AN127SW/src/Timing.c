//-----------------------------------------------------------------------------
// Timing.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for timing functions.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Common.h"
#include "Pins.h"
#include "Timing.h"
#include "Errors.h"

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

//INTERRUPT_PROTO (PCA0_ISR, INTERRUPT_PCA0);
U8 Wait_us (U16 us);
U8 Wait_ms (U16 ms);
U8 Start_Stopwatch (void);
U8 Stop_Stopwatch (void);
U8 Set_Timeout_ms_1 (U16);
U8 Set_Timeout_us_1 (U16);


//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

//SBIT (TFus, SFR_TCON, 5);              // microsecond delay flag (TF0)
bit TFms;                              // millisecond delay flag
bit Timeout_ms_1;                      // millisecond timeout flag 1
bit Stopwatch_active;                  // '1' if Stopwatch_ms is counting

//bit Timeout_us_1;                      // microsecond timeout flag 1
U16 Freerun_us_High;                   // PCA main counter overflow
U16 Stopwatch_us;                      // stopwatch value in us

/*
U8  LED1_State;
U16 Stopwatch_ms;                      // stopwatch value in ms
U16 Freerun_ms;                        // free-running ms counter
U16 Timeout_ms_1_target;
*/

SEGMENT_VARIABLE (LED1_State, U8, SEG_PDATA);
SEGMENT_VARIABLE (Stopwatch_ms, U32, SEG_PDATA);
SEGMENT_VARIABLE (Freerun_ms, U16, SEG_PDATA);
SEGMENT_VARIABLE (Timeout_ms_1_target, U16, SEG_PDATA);

//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timekeeping Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Wait_us
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : number of us to wait
//
// This value waits for TFus to go from 0 to 1, at least <us> times.
//
//-----------------------------------------------------------------------------
U8 Wait_us (U16 us)
{
   TFus = 0;                           // clear us overflow flag
   while (us != 0x0000)                // repeat until <us> have elapsed
   {
      while (!TFus);                   // wait for us flag to be set
      TFus = 0;                        // clear it
      us--;                            // update count
   }
   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Wait_ms
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : number of ms to wait
//
// This value waits for TFms to go from 0 to 1, at least <ms> times.
//
//-----------------------------------------------------------------------------
U8 Wait_ms (U16 ms)
{
   TFms = 0;                           // clear ms overflow flag
   while (ms != 0x0000)                // repeat until <ms> have elapsed
   {
      while (!TFms);                   // wait for ms flag to be set
      TFms = 0;                        // clear it
      ms--;                            // update count
   }
   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Start_Stopwatch
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : None
//
// This function starts the Stopwatch function.
//
//-----------------------------------------------------------------------------
U8 Start_Stopwatch (void)
{
   UU16 temp;
   Stopwatch_ms = 0;
   Stopwatch_active = 1;
   temp.U8[LSB] = PCA0L;
   temp.U8[MSB] = PCA0H;
   Stopwatch_us = temp.U16;            // set Stopwatch_us to current PCA0
                                       // count
   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Stop_Stopwatch
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : None
//
// This function stops the Stopwatch function.
//
//-----------------------------------------------------------------------------
U8 Stop_Stopwatch (void)
{
   UU16 temp;

   Stopwatch_active = 0;
   temp.U8[LSB] = PCA0L;               // get current PCA0 count
   temp.U8[MSB] = PCA0H;
   // subtract current count from previous count to get elapsed time in us.
   Stopwatch_us = temp.U16 - Stopwatch_us;
   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Set_Timeout_ms_1
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : number of ms until Timeout_ms_1 will go to a '1'
//
// This function configures the ms timeout.
//
//-----------------------------------------------------------------------------
U8 Set_Timeout_ms_1 (U16 ms)
{
   U16 local_Freerun;
   static bit EA_SAVE;

   EA_SAVE = EA;
   DISABLE_INTERRUPTS ();              // disable interrupts
   local_Freerun = Freerun_ms;
   EA = EA_SAVE;

   Timeout_ms_1_target = local_Freerun + ms;

   Timeout_ms_1 = 0;                   // Clear timeout flag

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Set_Timeout_us_1
//-----------------------------------------------------------------------------
//
// Return Value : error code
// Parameters   : number of us until Timeout_us_1 will go to a '1'
//
// This function configures the us timeout.
//
//-----------------------------------------------------------------------------
U8 Set_Timeout_us_1 (U16 us)
{
   UU16 temp;                          // variable for temporary math

//   PCA0CPM4 &= ~0x01;                  // disable Module 4 interrupts
//   PCA0CPM4 &= ~0x40;                  // disable further matches
//   CCF4 = 0;                           // clear pending interrupts

   // set Module 4 match register to 'us' counts from current PCA counter
   // position
   temp.U8[LSB] = PCA0L;               // read current counter value LSB
                                       // first
   temp.U8[MSB] = PCA0H;
   temp.U16 = temp.U16 + us;           // calculate target match value
   PCA0CPL4 = temp.U8[LSB];            // repaint match value LSB first
   PCA0CPH4 = temp.U8[MSB];

   CCF4 = 0;                           // clear any pending matches

//   Timeout_us_1 = 0;                   // Clear timeout flag

//   PCA0CPM4 |= 0x01;                   // enable Module 4 interrupts

   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA0_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Use PCA0 as follows:
//  * Main time base clocked from Timer0 overflows, and counts us.
//  * Main counter interrupt enabled
//  * Module 5 set in Software Timer mode, interrupts enabled here,
//    set to interrupt at 1 ms intervals (ms counter, ms Timeout)
//    * will always set TFms
//    * will increment Stopwatch_ms if Stopwatch_active is '1'
//    * will always increment Freerun_ms
//    * will set Timeout_ms_1 if match occurs between Freerun_ms and
//      Timeout_ms_1_target
//  * Module 4 set in Software Timer mode, interrupts enabled elsewhere,
//    * will set Timeout_us_1 and clear disable its own interrupts
//
//-----------------------------------------------------------------------------
INTERRUPT (PCA0_ISR, INTERRUPT_PCA0)
{
   UU16 temp;                          // temporary math register

   // handle all flags

   if (CCF0)                           // handle CCF0
   {
      CCF0 = 0;
   }
   else if (CCF1)                      // handle CCF1
   {
      CCF1 = 0;
   }
   else if (CCF2)                      // handle CCF2
   {
      CCF2 = 0;
   }
   else if (CCF3)                      // handle CCF3
   {
      CCF3 = 0;
      TFms = 1;                        // set the ms counter bit
      Freerun_ms++;                    // update MS counter
      if (Stopwatch_active)
      {
         Stopwatch_ms++;               // update stopwatch if active
      }
      if (Freerun_ms == Timeout_ms_1_target)
      {
         Timeout_ms_1 = 1;
      }
      // update match for next ms
      temp.U8[LSB] = PCA0CPL3;         // read last match value
      temp.U8[MSB] = PCA0CPH3;
      // add 1000
      temp.U16 = temp.U16 + 1000;
      // re-write, Low byte first
      PCA0CPL3 = temp.U8[LSB];
      PCA0CPH3 = temp.U8[MSB];
      if (LED1_State == LED_UART_RX_TX)
      {
         if ((TX0 == LOW) || (RX0 == LOW))
         {
            LED1_OFF ();
         }
         else
         {
            LED1_ON ();
         }
      }
   }
/*
   // used in polled mode now
   else if (CCF4)                      // handle CCF4
   {
      CCF4 = 0;
      Timeout_us_1 = 1;
      PCA0CPM4 &= ~0x40;               // disable further match events
      PCA0CPM4 &= ~0x01;               // disable further interrupts
   }
*/
   else if (CF)                        // handle CF
   {
      CF = 0;
      Freerun_us_High++;               // update overflow counter
      if (LED1_State == LED_ON)
      {
         LED1_ON ();
      }
      else if (LED1_State == LED_OFF)
      {
         LED1_OFF ();
      }
      else if (LED1_State == LED_FLASHING)
      {
         LED1_SWITCH ();
      }
   }
}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

