//-----------------------------------------------------------------------------
// Init.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for initialization functions.
//
//-----------------------------------------------------------------------------

#ifndef INIT_H
#define INIT_H

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern void WDT_Init (void);
extern void SYSCLK_Init (void);
extern void UART0_Init (void);
extern void PORT_Init (void);
extern void Timer0_Init (void);
extern void PCA0_Init (void);

#endif // INIT_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

