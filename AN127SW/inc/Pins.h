//-----------------------------------------------------------------------------
// Pins.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for C2 and JTAG interface pins and
// timing.
//
// The pinout is optimized to map the standard Port I/O header to the
// standard 10-pin debug connector, as follows:
// Pin #   C2 debug   JTAG debug   F380 pin     F380 configuration
// Pin 1   +3V        +3V          P3.0         PP high -- overdrive +3V node
// Pin 2   GND        GND          P3.1         PP low -- simulate ground
// Pin 3   GND        GND          P3.2         PP low -- simulate ground
// Pin 4   C2D        TCK          P3.3         OD high
// Pin 5   /RST_share TMS          P3.4         OD high
// Pin 6   C2D_share  TDO          P3.5         OD high
// Pin 7   C2CK       TDI          P3.6         OD high
// Pin 8   VPPEN      VPPEN        P3.7         OD high
// Pin 9   GND        GND          +3.3V        ** MUST CUT stake header! **
// Pin 10  VBUS       NC           GND          no connect
//
// LED1 is P2.2
// TX0 is P0.4
// RX0 is P0.5
// RTS is P2.6
// CTS is P2.7
//
// #EFM8UB2
// STK Expansion Header:
// Pin #   C2 debug   JTAG debug   UB2 pin      UB2 configuration
// Pin 3   C2D        TCK          P3.0         OD high
// Pin 5   /RST_share TMS          P3.1         OD high
// Pin 7   C2CK  	  TDO          P3.2         OD high
// Pin 9   C2D_share  TDI          P3.3         OD high
//
// LED1 is P2.2
// TX0 is P0.4
// RX0 is P0.5
// RTS is P2.6
// CTS is P2.7
//
//-----------------------------------------------------------------------------

#ifndef PINS_H
#define PINS_H

#include "Common.h"
#include <compiler_defs.h>
#include <C8051F380_defs.h>

#define EFM8UB2 0
#define C8051F380 1

#define DEVICE EFM8UB2
//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

U8 Pin_Init (void);
U8 Pin_Test (void);

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------
#if DEVICE == C8051F380	// C8051F380
SBIT (C2D, SFR_P3, 3);
SBIT (C2CK, SFR_P3, 6);
SBIT (VPP_65, SFR_P3, 7);

SBIT (TCK, SFR_P3, 3);
SBIT (TMS, SFR_P3, 4);
SBIT (TDO, SFR_P3, 5);
SBIT (TDI, SFR_P3, 6);

SBIT (LED1, SFR_P2, 2);
SBIT (TX0, SFR_P0, 4);
SBIT (RX0, SFR_P0, 5);

#define C2D_DRIVER_ON() P3MDOUT |= 0x08

#define C2D_DRIVER_OFF() P3 |= 0x08; \
                         P3MDOUT &= ~0x08

#define C2CK_DRIVER_ON() P3MDOUT |= 0x40

#define C2CK_DRIVER_OFF() P3 |= 0x40; \
                          P3MDOUT &= ~0x40

#define LED1_ON()    LED1 = HIGH
#define LED1_OFF()   LED1 = LOW
#define LED1_SWITCH() LED1 = !LED1

#define VPP_65_DRIVER_ON() P3 &= ~0x80; \
                           P3MDOUT |= 0x80

#define VPP_65_DRIVER_OFF() P3 |= 0x80; \
                            P3MDOUT &= ~0x80
#elif DEVICE == EFM8UB2 // EFM8UB2
SBIT (C2D, SFR_P3, 0);
SBIT (C2CK, SFR_P3, 2);
SBIT (VPP_65, SFR_P3, 5);

SBIT (TCK, SFR_P3, 0);
SBIT (TMS, SFR_P3, 1);
SBIT (TDO, SFR_P3, 4);
SBIT (TDI, SFR_P3, 3);

SBIT (LED1, SFR_P2, 0);
SBIT (TX0, SFR_P0, 4);
SBIT (RX0, SFR_P0, 5);

#define C2D_DRIVER_ON() P3MDOUT |= 0x01

#define C2D_DRIVER_OFF() P3 |= 0x01; \
                         P3MDOUT &= ~0x01

#define C2CK_DRIVER_ON() P3MDOUT |= 0x4

#define C2CK_DRIVER_OFF() P3 |= 0x4; \
                          P3MDOUT &= ~0x4

#define LED1_ON()    LED1 = LOW
#define LED1_OFF()   LED1 = HIGH
#define LED1_SWITCH() LED1 = !LED1

#define VPP_65_DRIVER_ON() P3 &= ~0x80; \
                           P3MDOUT |= 0x80

#define VPP_65_DRIVER_OFF() P3 |= 0x80; \
                            P3MDOUT &= ~0x80

#endif
enum {LED_OFF, LED_ON, LED_FLASHING, LED_UART_RX_TX};

//-----------------------------------------------------------------------------
// Exported macros
//-----------------------------------------------------------------------------





#define VPP_65_ENABLE() VPP_65 = HIGH
#define VPP_65_DISABLE() VPP_65 = LOW

#define SET_C2D_HIGH() C2D = HIGH
#define SET_C2D_LOW() C2D = LOW
#define SET_C2CK_HIGH() C2CK = HIGH
#define SET_C2CK_LOW() C2CK = LOW

#define JTAG_DRIVERS_ON() //P3MDOUT |= 0x58; \
                          //P3 &= ~0x58

#define JTAG_DRIVERS_OFF() //P3 |= 0x58; \
                           //P3MDOUT &= 0x58

#define STROBE_TCK() TCK = LOW; TCK = HIGH; TCK = LOW

/*
#define STROBE_TCK() TCK = LOW;  TCK = LOW; TCK = LOW; TCK = LOW; \
                     TCK = HIGH; TCK = HIGH; TCK = HIGH; TCK = HIGH; \
                     TCK = LOW;  TCK = LOW; TCK = LOW; TCK = LOW
*/
#define JTAG_ANY_TO_RTI()   TMS = HIGH; STROBE_TCK(); STROBE_TCK(); \
                            STROBE_TCK(); STROBE_TCK(); STROBE_TCK(); \
                            TMS = LOW; STROBE_TCK()

#define JTAG_RTI_TO_SHIFTDR() TMS = HIGH; STROBE_TCK(); \
                              TMS = LOW;  STROBE_TCK(); \
                              STROBE_TCK()

#define JTAG_RTI_TO_SHIFTIR() TMS = HIGH; STROBE_TCK(); \
                              STROBE_TCK(); \
                              TMS = LOW;  STROBE_TCK(); \
                              STROBE_TCK()

#define JTAG_EXIT_TO_RTI()    TMS = HIGH; STROBE_TCK(); \
                              TMS = LOW;  STROBE_TCK()

/*
#define  STROBE_C2CK()    C2CK = LOW ; \
                          C2CK = HIGH; C2CK = HIGH; C2CK = HIGH;//C2CK = HIGH;C2CK = HIGH;C2CK = HIGH
*/


// this works for 100 MHz system clock; could be changed for 48 MHz or sysclk-dependent value

#define  STROBE_C2CK()    C2CK = HIGH;C2CK = HIGH;C2CK = LOW ;C2CK = LOW ;C2CK = LOW ;C2CK = LOW; \
                          C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH

/*
#define  STROBE_C2CK()     C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = LOW ;C2CK = LOW ; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH
*/



// really slow C2CK timing...
/*
#define  STROBE_C2CK()    C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = LOW ;C2CK = LOW ; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH; \
                           C2CK = HIGH;C2CK = HIGH;C2CK = HIGH;C2CK = HIGH
*/

#endif // PINS_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

