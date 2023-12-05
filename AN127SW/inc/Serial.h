//-----------------------------------------------------------------------------
// Serial.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for serial in/out operations.
//
//-----------------------------------------------------------------------------

#ifndef SERIAL_H
#define SERIAL_H

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Common.h"

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern S8 * GETS (S8 *buf, U16 len);
extern char _getkey ();
extern char getchar (void);

extern U16 atox (S8 * s);
extern U32 atolx (S8 * s);

extern void UART_RX_FIFO_Init (void);
extern void UART_RX_FIFO_Enable (void);
extern void UART_RX_FIFO_Disable (void);

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------

SBIT (RTS_pin, SFR_P2, 6);
SBIT (CTS_pin, SFR_P2, 7);

#define UART_RX_FIFO_BUFSIZE           128
#define UART_RX_FIFO_INTERRUPT_NUMBER  INTERRUPT_UART0
#define UART_RX_FIFO_INTERRUPT_SFR     IE
#define UART_RX_FIFO_INTERRUPT_MASK    0x10
#define UART_RX_FIFO_SFRPAGE           0x00


//-----------------------------------------------------------------------------
// Exported macros
//-----------------------------------------------------------------------------

#define ASSERT_READY_TO_SEND()   RTS_pin = LOW

#define DEASSERT_READY_TO_SEND() RTS_pin = HIGH

#define CTS_CHECK()              CTS_bit = !CTS_pin

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

extern bit UART_RX_FIFO_READY;         // TRUE when data is available in FIFO
extern bit CTS_bit;
extern bit UART_RX_FIFO_OVR;           // TRUE when data has overrun FIFO
extern bit UART;                       // '0' for UART0, '1' for UART1
extern bit GETS_QUIET;                 // if 'TRUE', GETS will not echo
                                       // received characters
#endif // SERIAL_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

