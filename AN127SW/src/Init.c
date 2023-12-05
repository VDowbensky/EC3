//-----------------------------------------------------------------------------
// Init.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for initialization functions.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Common.h"
#include "Init.h"
#include "Pins.h"
#include "Serial.h"

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

#define BAUDRATE (115200L)             // UART baud rate in bps
//#define BAUDRATE (230400L)             // UART baud rate in bps

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

void WDT_Init (void);
void SYSCLK_Init (void);
void UART0_Init (void);
void PORT_Init (void);
void Timer0_Init (void);
void PCA0_Init (void);

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
// Initialization Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// WDT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine disables the WDT.
//
//-----------------------------------------------------------------------------
void WDT_Init (void)
{
   PCA0MD &= ~0x40;
}

//-----------------------------------------------------------------------------
// SYSCLK_Init 12 MHz intosc * 4 = 48 MHz
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This selects the clock multiplier as the system clock source.  The input
// to the multiplier is the 12 MHz internal oscillator.
// Also sets Flash read timing for 50 MHz operation.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
    U16 i = 0;

    FLSCL     = 0x90;                  // Set Flash read timing for 50 MHz

    OSCICN    = 0x83;                  // Enable intosc for div 1 mode

    CLKMUL    = 0x80;                  // Enable clkmul

    for (i = 0; i < 20; i++);          // Wait at least 5us for init

    CLKMUL    |= 0xC0;

    while ((CLKMUL & 0x20) == 0);

    CLKSEL    = 0x03;                  // enable CLKMUL as sysclk
}


/*
//-----------------------------------------------------------------------------
// SYSCLK_Init 24.5 MHz intosc * 4 = 98 MHz
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This selects the PLL as the system clock source.  The input to the PLL is
// the 24.5 MHz internal oscillator, and the PLL is configured to multiply
// by 4.  Also sets Flash read timing for 100 MHz operation.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 SFRPAGE_SAVE = SFRPAGE;          // preserve SFRPAGE
   U16 i;

   SFRPAGE = CONFIG_PAGE;
   OSCICN = 0x83;                      // INTOSC enabled at 24.5MHz
   CCH0CN &= ~0x20;                    // disable PREFETCH while modifying
                                       // FLRD bits

   // Start PLL for 100MHz operation
   SFRPAGE = PLL0_PAGE;
   PLL0CN = 0x00;                      // Select INTOSC as clk source
   SFRPAGE = LEGACY_PAGE;
   FLSCL &= ~0x30;                     // set FLASH read timing for 100MHz
   FLSCL |=  0x30;
   SFRPAGE = PLL0_PAGE;
   PLL0CN |= 0x01;                     // Enable PLL power
   PLL0DIV = 0x01;                     // divide by 1
   PLL0FLT &= ~0x0F;                   // set Loop Filt for (25)MHz input clock
   PLL0FLT |=  0x01;
   PLL0FLT &= ~0x30;                   // set ICO for 65-100MHz
   PLL0FLT |=  0x00;

   PLL0MUL = 0x04;                     // multiply by 4

   // wait at least 5us
   for (i = 0; i < 256; i++) ;

   PLL0CN |= 0x02;                     // Enable PLL

   while ((PLL0CN & 0x10) == 0x00);    // wait for PLL to lock

   SFRPAGE = CONFIG_PAGE;

   CLKSEL = 0x02;                      // select PLL as SYSCLK source

   CCH0CN |=  0x20;                    // re-enable PREFETCH

   SFRPAGE = SFRPAGE_SAVE;             // restore SFRPAGE
}

*/

//-----------------------------------------------------------------------------
// PORT_Init UART0
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ports as follows:
// P0.4 - TX0 (push-pull)   -- UART0 TX pin
// P0.5 - RX0 (open-drain)  -- UART0 RX pin
//
// P2.0 - VPP_65 (push-pull) -- OTP VPP programming voltage
//
// P2.2 - LED (push-pull)   -- status indicator
//
// P2.6 - RTS (push-pull)   UART h/w handshake output
// P2.7 - CTS (open-drain)  UART h/w handshake input
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
// Pin 9   GND        GND          GND
// Pin 10  VBUS       NC           -
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDOUT  = 0x10;                    // TX0 is push-pull

   P2MDOUT |= 0x01;                    // VPP_65 is push-pull
   P2MDOUT |= 0x04;                    // LED is push-pull
   P2MDOUT |= 0x40;                    // RTS is push-pull

   P3 = 0xF9;                          // init P3 state
   P3MDOUT = 0x07;                     //  and direction

   XBR0 = 0x01;                        // enable UART0
   XBR1 = 0x40;                        // enable Crossbar and weak pull-ups

   VPP_65_DISABLE ();                  // VPP_65 is disabled
   VPP_65_DRIVER_OFF ();
}

//-----------------------------------------------------------------------------
// UART_Init (defs)
//-----------------------------------------------------------------------------
//
// Configure the UART0 using Timer1 for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init(void)
{
    SCON0 = 0x10;                      // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
#if (SYSCLK/BAUDRATE/2/256 < 1)
    TH1 = -(SYSCLK/BAUDRATE/2);
    CKCON |=  0x08;                    // T1M = 1; SCA1:0 = xx
#elif (SYSCLK/BAUDRATE/2/256 < 4)
    TH1 = -(SYSCLK/BAUDRATE/2/4);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 01
    CKCON |=  0x01;
#elif (SYSCLK/BAUDRATE/2/256 < 12)
    TH1 = -(SYSCLK/BAUDRATE/2/12);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 00
#elif (SYSCLK/BAUDRATE/2/256 < 48)
    TH1 = -(SYSCLK/BAUDRATE/2/48);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 10
    CKCON |=  0x02;
#else
#error Unsupported SYSCLK/BAUDRATE combination.
#endif

    TL1 = TH1;                         // Initialize Timer1
    TMOD &= ~0xF0;                     // TMOD: Timer1 in 8-bit autoreload
    TMOD |=  0x20;
    TR1 = 1;                           // START Timer1
    TI0 = 1;                           // Indicate TX0 ready
}


//-----------------------------------------------------------------------------
// Timer0_Init   8-bit autoreload at 1 us
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer0 for 8-bit autoreload at 1us, no interrupts enabled.
//
//-----------------------------------------------------------------------------
void Timer0_Init (void)
{
   TCON &= ~0x30;                      // Stop timer and clear flags
   TMOD &= ~0x0F;                      // set mode to 8-bit autoreload
   TMOD |=  0x02;

   CKCON |= 0x04;                      // Timer counts SYSCLKs

   TH0 = (U8) -(SYSCLK / 1000000);     // set to reload at 1 us interval
   TL0 = TH0;

   ET0 = 0;                            // interrupts disabled

   TR0 = 1;                            // Start timer
}

//-----------------------------------------------------------------------------
// PCA0_Init   Timer0 is clock source
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure PCA0 as follows:
//  * Main time base clocked from Timer0 overflows
//  * Main counter interrupt enabled to provide 32-bits of freerunning us
//    count
//  * Module 3 set in Software Timer mode, interrupts enabled here,
//    set to interrupt at 1 ms intervals (ms counter, ms Timeout)
//  * Module 4 set in Software Timer mode, interrupts enabled elsewhere,
//     set to interrupt on match (us Timeout) or CC4 for used to poll for
//     match
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   UU16 temp;                          // temporary math register

   PCA0CN = 0x00;                      // Stop PCA counter; clear all flags
   PCA0MD = 0x05;                      // PCA counts in IDLE mode; Timer0
                                       // is clock source; PCA counter
                                       // interrupts are enabled
   PCA0L = 0x00;                       // initialize main counter
   PCA0H = 0x00;

   PCA0CPM3 = 0x49;                    // Module 3 in Software Timer mode
                                       //  with interrupts enabled
   // configure Module 3 to interrupt in 1000 us from now
   // PCA0CP3 = PCA0 + 1000
   temp.U8[LSB] = PCA0L;               // read Low byte first
   temp.U8[MSB] = PCA0H;
   temp.U16 = temp.U16 + 1000;         // add 1000
   PCA0CPL3 = temp.U8[LSB];            // write Low byte first
   PCA0CPH3 = temp.U8[MSB];            // followed by High byte

   PCA0CPM4 = 0x48;                    // Module 4 in Software Timer mode
                                       //  with interrupts disabled

   CR = 1;                             // Start PCA0 counter

   EIE1 |= 0x10;                       // Enable PCA0 interrupts

}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

