//-----------------------------------------------------------------------------
// Serial.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for serial in/out routines.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include <stdio.h>
#include <ctype.h>
#include "Common.h"
#include "Serial.h"

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

S8 * GETS (S8 *buf, U16 len);
char _getkey ();
char getchar (void);

U16 atox (S8 * s);
U32 atolx (S8 * s);

void UART_RX_FIFO_Init (void);
void UART_RX_FIFO_Enable (void);
void UART_RX_FIFO_Disable (void);


//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

bit UART_RX_FIFO_READY;                // TRUE when data is available in FIFO
bit CTS_bit;                           // TRUE when it is okay for us to TX
bit UART_RX_FIFO_OVR;                  // TRUE when data has overrun FIFO
bit UART;                              // '0' for UART0, '1' for UART1
bit GETS_QUIET;                        // if TRUE, received characters are
                                       // not echoed.

//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

unsigned char UART_RX_FIFO_Get (void);

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

/*
static U8 UART_RX_FIFO_Head;
static U8 UART_RX_FIFO_Tail;
static U8 UART_RX_FIFO_Count;
static U8 UART_RX_FIFO_MaxCount;
*/

SEGMENT_VARIABLE (UART_RX_FIFO_Head, U8, SEG_PDATA);
SEGMENT_VARIABLE (UART_RX_FIFO_Tail, U8, SEG_PDATA);
SEGMENT_VARIABLE (UART_RX_FIFO_Count, U8, SEG_PDATA);
SEGMENT_VARIABLE (UART_RX_FIFO_MaxCount, U8, SEG_PDATA);

static SEGMENT_VARIABLE (UART_RX_FIFO_Buf[UART_RX_FIFO_BUFSIZE], U8, SEG_XDATA);
static bit UART_TX_READY;              // TRUE when data has been transmitted


//-----------------------------------------------------------------------------
// Serial and Support Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Support Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// atox
//-----------------------------------------------------------------------------
//
// Return Value : integer value of hex string pointed to by <s>
// Parameters   : <s> source string
//
// This is a coarse port from "_atoi.c" by Sandeep Dutta, from SDCC build
// ver 2.7.0.
//
// This function returns an integer which is the hexadecimal representation
// of the string pointed to by <s>.
//-----------------------------------------------------------------------------

U16 atox (S8 * s)
{
   register U16 rv=0;
   register U8 hex_val;

   /* skip till we find either a hex digit */
   while (*s)
   {
      if (isxdigit (*s))
      {
         break;
      }
      s++;
   }

   while (*s && isxdigit(*s))
   {
      if ((*s >= '0') && (*s <= '9'))
      {
         hex_val = *s - '0';
      }
      else if ((*s >= 'a') && (*s <= 'f'))
      {
         hex_val = *s - ('a' - 10);
      }
      else
      {
         hex_val = 0;
      }
      rv = (rv * 16) + hex_val;
      s++;
   }

   return rv;
}

//-----------------------------------------------------------------------------
// atolx
//-----------------------------------------------------------------------------
//
// Return Value : 32-bit value of hex string pointed to by <s>
// Parameters   : <s> source string
//
// This is a coarse port from "_atoi.c" by Sandeep Dutta, from SDCC build
// ver 2.7.0.
//
// This function returns a long which is the hexadecimal representation
// of the string pointed to by <s>.
//-----------------------------------------------------------------------------

U32 atolx (S8 * s)
{
   U32 rv=0;
   register U8 hex_val;

   /* skip till we find either a hex digit */
   while (*s)
   {
      if (isxdigit (*s))
      {
         break;
      }
      s++;
   }

   while (*s && isxdigit(*s))
   {
      if ((*s >= '0') && (*s <= '9'))
      {
         hex_val = *s - '0';
      }
      else if ((*s >= 'a') && (*s <= 'f'))
      {
         hex_val = *s - ('a' - 10);
      }
      else
      {
         hex_val = 0;
      }
      rv = (rv * 16) + hex_val;
      s++;
   }

   return rv;
}


//-----------------------------------------------------------------------------
// GETS
//-----------------------------------------------------------------------------
//
// Return Value : Pointer to string containing buffer read from UART.
// Parameters   : <buf> to store string; maximum <n>umber of characters to
//                read; <n> must be 2 or greater.
//
// This function returns a string of maximum length <n>.
//-----------------------------------------------------------------------------
S8 * GETS (S8 *buf, U16 len)
{
   U8 temp;
   U8 i;
   U8 done;

   done = FALSE;
   i = 0;

   while (done == FALSE)
   {
      if (GETS_QUIET == FALSE)
      {
         temp = getchar ();
      }
      else
      {
         temp = _getkey ();
      }

      if (temp == '\b')
      {
         if (i != 0)                   // backspace if possible
         {
            i = i - 1;
//            putchar ('\b');
         }
      }
      else
      // handle newline
      if ((temp == '\n') || (temp == '\r'))
      {
         buf[i] = '\0';                // add null terminator to string
         done = TRUE;
      }
      else
      if (temp == '\0')                // handle EOF
      {
         buf[i] = '\0';                // add null terminator to string
      }
      else                             // handle new character
      {
         buf[i] = temp;
//         putchar (temp);               // echo character
         i = i + 1;
         if (i == (len-1))
         {
            buf[i] = '\0';
            done = TRUE;
          }
       }
   }

   return buf;
}

//-----------------------------------------------------------------------------
// getchar
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function returns a character from the UART.
//
//-----------------------------------------------------------------------------
char getchar (void)
{
   U8 c;
   c = _getkey ();
   putchar (c);

   return c;
}

//-----------------------------------------------------------------------------
// UART_RX_FIFO Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART_RX_FIFO_Init
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function initializes the UART RX FIFO, clears all counters, and
// enables the FIFO.  Assumes that the UART has been initialized elsewhere.
//
void UART_RX_FIFO_Init (void)
{
   UART_RX_FIFO_Disable ();
   UART_RX_FIFO_Head = 0;
   UART_RX_FIFO_Tail = 0;
   UART_RX_FIFO_Count = 0;
   UART_RX_FIFO_MaxCount = 0;
   UART_RX_FIFO_OVR = 0;
   UART_RX_FIFO_READY = 0;
   UART_TX_READY = 1;
   UART_RX_FIFO_Enable ();
   ASSERT_READY_TO_SEND ();
   GETS_QUIET = FALSE;
}

//-----------------------------------------------------------------------------
// UART_RX_FIFO_Enable
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function enables the UART RX FIFO by enabling its UART interrupt.
//
void UART_RX_FIFO_Enable (void)
{
   UART_RX_FIFO_INTERRUPT_SFR |= UART_RX_FIFO_INTERRUPT_MASK;
}

//-----------------------------------------------------------------------------
// UART_RX_FIFO_Disable
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function disables the UART RX FIFO by disabling its UART interrupt.
//
void UART_RX_FIFO_Disable (void)
{
   UART_RX_FIFO_INTERRUPT_SFR &= ~UART_RX_FIFO_INTERRUPT_MASK;
}

//-----------------------------------------------------------------------------
// UART_RX_FIFO_Get
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function returns a character from the UART RX FIFO.
//
U8 UART_RX_FIFO_Get (void)
{
   U8 retval;                          // value to return

   retval = 0x00;

   if (UART_RX_FIFO_READY)
   {                                   // if a character is available

      UART_RX_FIFO_Disable ();

      retval = UART_RX_FIFO_Buf[UART_RX_FIFO_Tail];

      UART_RX_FIFO_Count--;

      if (UART_RX_FIFO_Count == 0)
      {
         UART_RX_FIFO_READY = 0;
         ASSERT_READY_TO_SEND ();
      }

      UART_RX_FIFO_Tail++;

      if (UART_RX_FIFO_Tail == UART_RX_FIFO_BUFSIZE)
      {
         UART_RX_FIFO_Tail = 0;
      }

      UART_RX_FIFO_Enable ();
   }

   return retval;
}

//-----------------------------------------------------------------------------
// UART_RX_FIFO_ISR
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function is called on UART_RX_FIFO interrupt service.  It adds the
// received character to the FIFO buffer.  New data will overrun old data in
// the buffer, and will set the OVR flag.
//
// Note that this routine will work for UART0 or UART1 on an SFRPAGE'd device.
//
INTERRUPT (UART_RX_FIFO_ISR, UART_RX_FIFO_INTERRUPT_NUMBER)
//void UART_RX_FIFO_ISR (void) interrupt UART_RX_FIFO_INTERRUPT_NUMBER
{
   if (RI0)
   {

      RI0 = 0;

      UART_RX_FIFO_Count++;

      if (UART_RX_FIFO_Count > UART_RX_FIFO_MaxCount)
      {
         UART_RX_FIFO_MaxCount = UART_RX_FIFO_Count;
      }

      if (UART_RX_FIFO_Count > (UART_RX_FIFO_BUFSIZE / 2))
      {
         DEASSERT_READY_TO_SEND ();
      }


      if ((unsigned) UART_RX_FIFO_Count > (unsigned) UART_RX_FIFO_BUFSIZE)
      {

         UART_RX_FIFO_Count = UART_RX_FIFO_BUFSIZE;
         UART_RX_FIFO_OVR = 1;

      }
      else
      {

         UART_RX_FIFO_Buf[UART_RX_FIFO_Head] = SBUF0;

         UART_RX_FIFO_Head++;
         if (UART_RX_FIFO_Head == UART_RX_FIFO_BUFSIZE)
         {
            UART_RX_FIFO_Head = 0;
         }


         if (UART_RX_FIFO_Count > UART_RX_FIFO_MaxCount)
         {
            UART_RX_FIFO_MaxCount = UART_RX_FIFO_Count;
         }
      }

      UART_RX_FIFO_READY = 1;          // indicate data available

   }
   else if (TI0)
   {
      TI0 = 0;
      UART_TX_READY = 1;
   }
}

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
/*
 * putchar (basic version): expands '\n' into CR LF
 */
// Modified by BW
//
// Return Value : character received from UART
// Parameters   : None
//
// This routine overloads the standard putchar() library function to test
// a software bit, <UART_TX_READY> instead of the hardware UART TX complete
// bit (TI).
//
#ifdef __C51__
char putchar (char c)
#elif defined SDCC
void putchar (char c)
#endif
{
   if (UART == 0)
   {
   }
   else
   {
   }

   if (c == '\n')  {
      while (!UART_TX_READY);
      UART_TX_READY = 0;
//      CTS_CHECK ();
//      while (CTS_bit == FALSE)
//      {
//         CTS_CHECK ();
//      }
      SBUF0 = 0x0d;                    /* output CR  */
   }
   while (!UART_TX_READY);
   UART_TX_READY = 0;

   SBUF0 = c;

#ifdef __C51__
   return c;
#elif defined SDCC

#endif
}

//-----------------------------------------------------------------------------
// _getkey
//-----------------------------------------------------------------------------
//
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
// Modified by BW
//
// Return Value : character received from UART
// Parameters   : None
//
// This routine overloads the standard _getkey() library function to get its
// data from an RX FIFO instead of the hardware UART.
//
char _getkey ()
{
   char c;

   while (!UART_RX_FIFO_READY);
   c = UART_RX_FIFO_Get ();

   return (c);
}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

