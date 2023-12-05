//-----------------------------------------------------------------------------
// HEX_Utils.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains several useful utilities for interfacing encoding and
// decoding HEX records
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include "Common.h"
#include "HEX_Utils.h"
#include "Errors.h"
#include <ctype.h>

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported Prototypes
//-----------------------------------------------------------------------------

U8 HEX_Encode (S8 *dest, HEX_RECORD *hexsrc, bit suppress);
U8 HEX_Decode (HEX_RECORD *hexdest, S8 *src);
U8 HEX2uc (S8 *src);
void uc2HEX (S8 *dest, U8 mychar);
void HEXSTR2BIN (S8 *dest, S8 *src, U8 *length_bytes);
void BIN2HEXSTR (S8 *dest, S8 *src, U8 length_bytes);

//-----------------------------------------------------------------------------
// Internal Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported HEX Functions
//-----------------------------------------------------------------------------

/*
//-----------------------------------------------------------------------------
// HEX_Decode
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : <hexdest> points to a HEX record to fill with the contents
//                pointed to by <src>.
//
//
// Hex Record Format:
//
// +--------+--------+------+-------+--------+------(n bytes)------+--------+
// | RECORD | RECLEN |    OFFSET    | RECORD |                     | CHKSUM |
// |  MARK  |  (n)   |   (2 BYTES)  |  TYPE  |        DATA         |        |
// |  ':'   |        |              |        |                     |        |
// +--------+--------+------+-------+--------+------(n bytes)------+--------+
//
// This routine fills <hexdest> with the contents of the HEX record pointed
// to by the string <src>.
//
// Returns FALSE on checksum error, END record, or unsupported record.  TRUE
// only if DATA record with correct checksum.
//
//-----------------------------------------------------------------------------
U8 HEX_Decode (HEX_RECORD *hexdest, S8 *src)
{
   U8 i;                               // loop counter
   U8 chksum = 0;
   U8 temp;
   U8 retval;

   retval = NO_ERROR;                  // assume no errors

   if (*src == ':')                    // check for starting delimiter
   {
      src++;
      temp = HEX2uc (src);             // get record length
      hexdest->RECLEN = temp;
      chksum += temp;
      src += 2;

      temp = HEX2uc (src);             // get offset (Address High)
      hexdest->OFFSET.U8[MSB] = temp;
      chksum += temp;
      src += 2;

      temp = HEX2uc (src);             // get offset (Address Low)
      hexdest->OFFSET.U8[LSB] = temp;
      chksum += temp;
      src += 2;

      temp = HEX2uc (src);             // get record type
      hexdest->RECTYP = temp;
      chksum += temp;
      src += 2;

      // read record DATA
      for (i = 0; i < hexdest->RECLEN; i++)
      {
         temp = HEX2uc (src);
         hexdest->Buf[i] = temp;
         chksum += temp;
         src += 2;
      }

      temp = HEX2uc (src);             // get checksum
      chksum = -chksum;                // invert checksum

      if (hexdest->RECTYP == HEXREC_EOF)
      {
         retval = EOF_HEX_RECORD;
      }
      else if (hexdest->RECTYP != HEXREC_DAT)
      {
         retval = UNSUPPORTED_HEX_RECORD;
      }
      else if (temp != chksum)
      {
         retval = BAD_CHECKSUM;
      }
   }
   else
   {
      retval = NOT_A_HEX_RECORD;
   }

   return retval;
}

*/

//-----------------------------------------------------------------------------
// HEX_Encode
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : <dest> points to binary array to fill with the contents
//                of <hexsrc>.  <suppress> if true will suppress trailing
//                0xff bytes.
//
// This routine fills <*dest> with a HEX record string matching the contents
// of the HEX record pointed to by <hexsrc>.
//
//-----------------------------------------------------------------------------
U8 HEX_Encode (S8 *dest, HEX_RECORD *hexsrc, bit suppress)
{
   U8 i;                               // loop counter
   U8 chksum;                          // checksum
   U8 temp;                            // temporary character

   *dest = ':';                        // start of record
   dest++;

   if (hexsrc->RECLEN != 0x00)
   {
      if (suppress)
      {                                // find end of record (first
                                       // non-0xFF byte
         i = hexsrc->RECLEN;
         i--;
         while ((hexsrc->Buf[i] == 0xFF) && (i != 0x00))
         {
            i--;
         }
         i++;
         hexsrc->RECLEN = i;
      }
   }

   temp = hexsrc->RECLEN;              // apply record length
   uc2HEX (dest, temp);
   chksum = temp;
   dest += 2;

   temp = hexsrc->OFFSET.U8[MSB];      // apply Address High
   uc2HEX (dest, temp);
   chksum += temp;
   dest += 2;

   temp = hexsrc->OFFSET.U8[LSB];      // apply Address Low
   uc2HEX (dest, temp);
   chksum += temp;
   dest += 2;

   temp = hexsrc->RECTYP;              // apply record type
   uc2HEX (dest, temp);
   chksum += temp;
   dest += 2;

   // copy all record bytes
   for (i = 0; i < hexsrc->RECLEN; i++)
   {
      temp = hexsrc->Buf[i];
      uc2HEX (dest, temp);
      chksum += temp;
      dest += 2;
   }

   chksum = -chksum;                   // invert chksum

   temp = chksum;                      // apply chksum
   uc2HEX (dest, temp);
   dest += 2;

   *dest = '\0';                       // NULL-terminate string

   return NO_ERROR;
}


//-----------------------------------------------------------------------------
// HEX2uc
//-----------------------------------------------------------------------------
//
// Return Value : U8 binary value
// Parameters   : *src points to a HEX string
//
//
// This routine takes 2 ASCII HEX characters pointed to by <src> and returns
// an unsigned char result.
//
//-----------------------------------------------------------------------------
U8 HEX2uc (S8 *src)
{
   U8 retval;
   U8 byte1, byte2;

   byte1 = toupper(src[0]);
   byte2 = toupper(src[1]);

   if (byte1 > '9')
   {
      byte1 = byte1 - 'A' + 10;
   }
   else
   {
      byte1 -= '0';
   }
   if (byte2 > '9')
   {
      byte2 = byte2 - 'A' + 10;
   }
   else
   {
      byte2 -= '0';
   }

   retval = (byte1 << 4) | byte2;

   return retval;
}



//-----------------------------------------------------------------------------
// uc2HEX
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : <mychar> contains source binary value. <*dest> points to
//                destination hex string.
//
//
// This routine takes an unsigned char <mychar> and copies 2 ASCII HEX
// characters to <dest>.
//
//-----------------------------------------------------------------------------
void uc2HEX (S8 *dest, U8 mychar)
{
   U8 byte1, byte2;

   byte1 = mychar >> 4;
   byte2 = mychar & 0x0F;

   if (byte1 > 9)
   {
      byte1 = byte1 + 'A' - 10;
   }
   else
   {
      byte1 += '0';
   }

   if (byte2 > 9)
   {
      byte2 = byte2 + 'A' - 10;
   }
   else
   {
      byte2 += '0';
   }

   *dest++ = byte1;
   *dest = byte2;
}


//-----------------------------------------------------------------------------
// HEXSTR2BIN
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : <src> points to a hex string, <dest> points to a binary
//                destination array, <length_bytes> is returned by reference
//                and is equal to the number of binary bytes copied into
//                dest.
//
// This routine decodes a hex string into a binary array.  Returns <length_
// bytes> by reference.
//
//-----------------------------------------------------------------------------
void HEXSTR2BIN (S8 *dest, S8 *src, U8 *length_bytes)
{
   U8 length;

   length = 0;
   while (isxdigit(*src))
   {
      *dest = HEX2uc (src);
      dest++;
      src += 2;
      length++;
   }
   *length_bytes = length;
}


//-----------------------------------------------------------------------------
// BIN2HEXSTR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : <src> points to a binary array, <dest> points to a hex
//                string, <length_bytes> is the number of binary bytes to
//                translate.
//
// This routine encodes a <length_bytes> bytes of a binary array into a hex
// string into a binary array.
//
//-----------------------------------------------------------------------------
void BIN2HEXSTR (S8 *dest, S8 *src, U8 length_bytes)
{
   U8 i;                               // byte counter

   for (i = 0; i < length_bytes; i++)
   {
      uc2HEX (dest, *src);
      dest += 2;
      src++;
   }
   *dest = '\0';                       // null-terminate string
}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

