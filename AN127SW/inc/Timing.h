//-----------------------------------------------------------------------------
// Timing.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for timming functions.
//
//-----------------------------------------------------------------------------

#ifndef TIMING_H
#define TIMING_H

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------

SBIT (Timeout_us_1, SFR_PCA0CN, 4);    // microsecond timeout flag 1

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern INTERRUPT_PROTO (PCA0_ISR, INTERRUPT_PCA0);
extern U8 Wait_us (U16 us);
extern U8 Wait_ms (U16 ms);
extern U8 Start_Stopwatch (void);
extern U8 Stop_Stopwatch (void);
extern U8 Set_Timeout_ms_1 (U16);
extern U8 Set_Timeout_us_1 (U16);


//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

SBIT (TFus, SFR_TCON, 5);              // microsecond delay flag (TF0)
extern bit TFms;                       // millisecond delay flag
extern bit Timeout_ms_1;               // millisecond timeout flag 1
extern bit Stopwatch_active;           // '1' if Stopwatch_ms is counting
//extern bit Timeout_us_1;               // microsecond timeout flag 1
extern U16 Freerun_us_High;            // PCA main counter overflow
extern U16 Stopwatch_us;               // stopwatch value in us

/*
extern U8 LED1_State;
extern U16 Stopwatch_ms;               // stopwatch value in ms
extern U16 Freerun_ms;                 // free-running ms counter
extern U16 Timeout_ms_1_target;
*/

extern SEGMENT_VARIABLE (LED1_State, U8, SEG_PDATA);
extern SEGMENT_VARIABLE (Stopwatch_ms, U32, SEG_PDATA);
extern SEGMENT_VARIABLE (Freerun_ms, U16, SEG_PDATA);
extern SEGMENT_VARIABLE (Timeout_ms_1_target, U16, SEG_PDATA);


#endif // TIMING_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

