//-----------------------------------------------------------------------------
// Common.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for project globals and macros.
//
//-----------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK (12000000L * 4)         // system clock frequency in Hz


#define VERSION "1.20"                 // version string
#define DEBUG_LEVEL 0                  // compile-time debug directive

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

//-----------------------------------------------------------------------------
// Global Macros
//-----------------------------------------------------------------------------

#define DISABLE_INTERRUPTS() EA = 0; EA = 0
#define ENABLE_INTERRUPTS() EA = 1

#endif // COMMON_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

