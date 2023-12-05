//-----------------------------------------------------------------------------
// Devices.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains JTAG/C2 device information.
//
//-----------------------------------------------------------------------------

#include "Common.h"
#include "Devices.h"
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Exported Global VARIABLES
//-----------------------------------------------------------------------------

SEGMENT_VARIABLE (FAMILY_NUMBER, U8, SEG_PDATA);
SEGMENT_VARIABLE (DERIVATIVE_NUMBER, U8, SEG_PDATA);
SEGMENT_VARIABLE (DEVICE_REV, U8, SEG_PDATA);
SEGMENT_VARIABLE (DERIVATIVE_REV, U8, SEG_PDATA);

bit FAMILY_FOUND;
bit DERIVATIVE_FOUND;
bit DEVICE_HALTED;


SEGMENT_VARIABLE (DUMMY_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x00, "C8051Fxxx", "DUMMY", "DUMMY", 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00},
   { 0x00, NULL,         NULL,          NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (DUMMY_INIT[], INIT_STRING, SEG_CODE) =
{
   NULL
};

// DERIVATIVE_ID            FEATURES          CODE_START      WRITELOCK     CODE2_START
//           DERIVATIVE_STRING           PACKAGE        CODE_SIZE       READLOCK          CODE2_SIZE

//DERIVATIVE_ENTRY code F30x_FAMILY[] =
SEGMENT_VARIABLE (F30x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x20, "C8051F300", "8K, SAR8, 2%osc",  "QFN11", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x21, "C8051F301", "8K, 2%osc",        "QFN11", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x22, "C8051F302", "8K, SAR8, 20%osc", "QFN11", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x23, "C8051F303", "8K, 20%osc",       "QFN11", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x26, "C8051F304", "4K, 20%osc",       "QFN11", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x27, "C8051F305", "2K, 20%osc",       "QFN11", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

//U8 * code F30x_INIT[] =
SEGMENT_VARIABLE (F30x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b2 07",                     // set intosc to 24.5 MHz
   NULL
};


//DERIVATIVE_ENTRY code F31x_FAMILY[] =
SEGMENT_VARIABLE (F31x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x50, "C8051F310", "16K, SAR10",     "QFP32", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x51, "C8051F311", "16K, SAR10",     "QFN28", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x52, "C8051F312", "8K, SAR10",      "QFP32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x53, "C8051F313", "8K, SAR10",      "QFN28", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x7b, "C8051F314", "8K, no ADC",     "QFP32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x7c, "C8051F315", "8K, no ADC",     "QFN28", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x38, "C8051F316-GM", "16K, SAR10",  "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x39, "C8051F317-GM", "16K, no ADC", "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F31x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd ef 00",                       // set SFRPAGE to 0x00
   "c2wd b2 83",                       // set intosc to 24.5 MHz
   NULL
};


SEGMENT_VARIABLE (F32x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x58, "C8051F320", "16K, SAR10",       "QFP32", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x59, "C8051F321", "16K, SAR10",       "QFN28", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F32x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b2 83",                     // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F326_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x54, "C8051F326", "16K",              "QFN28", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x55, "C8051F327", "16K",              "QFN28", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F326_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b2 83",                     // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F33x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x60, "C8051F330", "8K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x61, "C8051F331", "8K, no ADC or IDAC", "QFN20", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x2c, "C8051F332", "4K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x2d, "C8051F333", "4K, no ADC or IDAC", "QFN20", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x2e, "C8051F334", "2K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x2f, "C8051F335", "2K, no ADC or IDAC", "QFN20", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                    NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F33x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd b2 83",                       // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F35x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x62, "C8051F350", "8K, ADC24",  "QFP32", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x63, "C8051F351", "8K, ADC24",  "QFN28", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x64, "C8051F352", "8K, ADC16",  "QFP32", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x65, "C8051F353", "8K, ADC16",  "QFN28", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F35x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b6 10",                     // Set Flash read timing for 50 MHz
   "c2wsfr ab 00",                     // Reset Clk multiplier to intosc/2
   "c2wsfr be 80",                     // Enable Clk multiplier
   "wus 5",                            // wait for 5 us
   "c2wsfr be c0",                     // Initialize the multiplier
   "wms 10",                           // wait for 10 ms
   "c2rsfr be",                        // read clk multiplier value
   "c2wsfr a9 02",                     // Set clk mul as system clock
   NULL
};


SEGMENT_VARIABLE (F41x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x67, "C8051F410", "32K, SAR12, 2DACs",  "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x68, "C8051F411", "32K, SAR12, 2DACs",  "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x69, "C8051F412", "16K, SAR12, 2DACs",  "QFP32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x6a, "C8051F413", "16K, SAR12, 2DACs",  "QFN28", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x6f, "C8051F420", "32K, SAR10",         "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x70, "C8051F421", "32K, SAR10",         "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x71, "C8051F422", "16K, SAR10",         "QFP32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x72, "C8051F423", "16K, SAR10",         "QFN28", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F41x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b6 10",                     // Set Flash read timing for 50 MHz
   "c2wsfr c9 10",                     // Set Voltage regulator for 2.5 V
   "c2wsfr ff a0",                     // Enable VDD monitor at high threshold
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr b2 87",                     // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (Si8250_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x73, "Si8250-IM", "32K, 6-phase",  "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x74, "Si8250-IQ", "32K, 6-phase",  "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x75, "Si8251-IM", "16K, 6-phase",  "QFN28", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x76, "Si8251-IQ", "16K, 6-phase",  "QFP32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x77, "Si8252-IM", "16K, 3-phase",  "QFN28", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x78, "Si8252-IQ", "16K, 3-phase",  "QFP32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (Si8250_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b2 87",                     // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F34x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x7d, "C8051F340", "64K, 50 MHz, 2 UARTs",          "QFP48",       0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x7e, "C8051F341", "32K, 50 MHz, 2 UARTs",          "QFP48",       0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x7f, "C8051F342", "64K, 50 MHz, 1 UART",           "QFP32/QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x80, "C8051F343", "32K, 50 MHz, 1 UART",           "QFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x81, "C8051F344", "64K, 25 MHz, 2 UARTs",          "QFP48",       0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x82, "C8051F345", "32K, 25 MHz, 2 UARTs",          "QFP48",       0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x83, "C8051F346", "64K, 25 MHz, 1 UART, no LFO",   "QFP32/QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x84, "C8051F347", "32K, 25 MHz, 1 UART, no LFO",   "QFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xae, "C8051F348", "32K, 25 MHz, 2 UARTs, no adc",  "QFP48",       0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xaf, "C8051F349", "32K, 25 MHz, 1 UART, no adc",   "QFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x5a, "C8051F34A", "64K, 50 MHz, 2 UARTs",          "QFP32/QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x5b, "C8051F34B", "32K, 50 MHz, 2 UARTs",          "QFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                   NULL,                   NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F34x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b6 90",                     // Set Flash read timing for 50 MHz, one shot enabled
   "c2wsfr ff 80",                     // Enable VDD monitor
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr b9 00",                     // Reset Clk multiplier to intosc
   "c2wsfr b9 80",                     // Enable Clk multiplier
   "wus 5",                            // wait for 5 us
   "c2wsfr b9 c0",                     // Initialize the multiplier
   "wms 10",                           // wait for 10 ms
   "c2rsfr b9",                        // read clk multiplier value
   "c2wsfr a9 03",                     // Set clk mul as system clock
   NULL
};

SEGMENT_VARIABLE (F38x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xD0, "C8051F380", "64K, 48 MHz, 2 UARTs", "TQFP48",       0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xD1, "C8051F381", "64K, 48 MHz, 2 UARTs", "LQFP32/QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xD2, "C8051F382", "32K, 48 MHz, 2 UARTs", "TQFP48",       0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xD3, "C8051F383", "32K, 48 MHz, 2 UARTs", "LQFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xD4, "C8051F384", "64K, 48 MHz, 2 UARTs", "TQFP48",       0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xD5, "C8051F385", "64K, 48 MHz, 2 UARTs", "LQFP32/QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xD6, "C8051F386", "32K, 48 MHz, 2 UARTs", "TQFP48",       0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xD7, "C8051F387", "32K, 48 MHz, 2 UARTs", "LQFP32/QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xEC, "C8051F38C", "16K, 48 MHz, 2 UARTs", "LQFP32/QFN32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                   NULL,           NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F38x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b6 90",                     // Set Flash read timing for 48 MHz, one shot enabled
   "c2wsfr ff 80",                     // Enable VDD monitor
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 03",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (T60x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x85, "C8051T600", "8K, SAR10",  "QFN11/14SOIC", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x86, "C8051T601", "8K",         "QFN11/14SOIC", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x87, "C8051T602", "4K, SAR10",  "QFN11/14SOIC", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x88, "C8051T603", "4K",         "QFN11/14SOIC", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x89, "C8051T604", "2K, SAR10",  "QFN11/14SOIC", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x8a, "C8051T605", "2K",         "QFN11/14SOIC", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (T60x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b2 07",                     // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F52x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x8b, "C8051F520-IM/AM",        "8K, LIN",        "QFN11",        0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x8c, "C8051F521-IM",           "8K, no LIN",     "QFN11",        0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x8d, "C8051F523-IM/AM",        "4K, LIN",        "QFN11",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x8e, "C8051F524-IM",           "4K, no LIN",     "QFN11",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x8f, "C8051F526-IM/AM",        "2K, LIN",        "QFN11",        0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x90, "C8051F527-IM",           "2K, LIN",        "QFN11",        0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x91, "C8051F530-IM/AM",        "8K, LIN",        "QFN20",        0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x92, "C8051F531-IM",           "8K, no LIN",     "QFN20",        0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x93, "C8051F533-IM/AM",        "4K, LIN",        "QFN20",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x94, "C8051F534-IM",           "4K, no LIN",     "QFN20",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x95, "C8051F536-IM/AM",        "2K, LIN",        "QFN20",        0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x96, "C8051F537-IM",           "2K, LIN",        "QFN20",        0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x97, "C8051F530-IT/AT",        "8K, LIN",        "TSSOP20",      0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x98, "C8051F531-IT",           "8K, no LIN",     "TSSOP20",      0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0x99, "C8051F533-IT/AT",        "4K, LIN",        "TSSOP20",      0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x9a, "C8051F534-IT",           "4K, no LIN",     "TSSOP20",      0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x9b, "C8051F536-IT/AT",        "2K, LIN",        "TSSOP20",      0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x9c, "C8051F537-IT",           "2K, LIN",        "TSSOP20",      0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xd0, "C8051F520A-IM/AM",       "8K, LIN",       "QFN11",         0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F521A-IM",          "8K, no LIN",    "QFN11",         0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F523A-IM/AM",       "4K, LIN",       "QFN11",         0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F524A-IM",          "4K, no LIN",    "QFN11",         0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F526A-IM/AM",       "2K, LIN",       "QFN11",         0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xd5, "C8051F527A-IM",          "2K, LIN",       "QFN11",         0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xd6, "C8051F530A-IM/IT/AM/AT", "8K, LIN",       "QFN20/TSSOP20", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F531A-IM/IT",       "8K, no LIN",    "QFN20/TSSOP20", 0x0000L, 0x1DFFL, 0x1DFFL, 0x1DFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F533A-IM/IT/AM/AT", "4K, LIN",       "QFN20/TSSOP20", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F534A-IM/IT",       "4K, no LIN",    "QFN20/TSSOP20", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xda, "C8051F536A-IM/IT/AM/AT", "2K, LIN",       "QFN20/TSSOP20", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xdb, "C8051F537A-IM/IT",       "2K, LIN",       "QFN20/TSSOP20", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00, NULL,                     NULL,            NULL,            0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F52x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd b2 87",                       // set intosc to 24.5 MHz
   "c2wd ff a0",                       // enable VDD monitor to high setting
   NULL
};


SEGMENT_VARIABLE (F36x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x3a, "C8051F360-GQ", "100MHz, 32K, SAR10", "QFP48", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x3b, "C8051F361-GQ", "100MHz, 32K, SAR10", "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x3c, "C8051F362-GM", "100MHz, 32K, SAR10", "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x3d, "C8051F363-GQ", "100MHz, 32K"       , "QFP48", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x3e, "C8051F364-GQ", "100MHz, 32K"       , "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x3f, "C8051F365-GM", "100MHz, 32K"       , "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x5f, "C8051F366-GQ", "50MHz, 32K, SAR10" , "QFP32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x79, "C8051F367-GM", "50MHz, 32K, SAR10" , "QFN28", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0x7a, "C8051F368-GQ", "50MHz, 16K, SAR10" , "QFP32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x9d, "C8051F369-GM", "50MHz, 16K, SAR10" , "QFN28", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x00, NULL,           NULL,                  NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F36x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd a7 0f",                       // SFRPAGE = 0x0f;  Set SFRPAGE to 0xf
   "c2wd b7 83",                       // OSCICN = 0x83;   Set internal osc to 24.5 MHZ
   "c2wd 8f 00",                       // CLKSEL = 0x00;   Set sysclk to intosc
   "c2wd a7 00",                       // SFRPAGE = 0x00;  Set SFRPAGE to 0x0
   "c2wd ff 80",                       // VDM0CN = 0x80;  Enable VDD Monitor
   "wus 5",                            // wait for 5 us
   "c2wd ef 02",                       // RSTSRC = 0x02;  Select VDD Monitor as a reset source.
/*
   // The following is the sequence to operate from a 98 MHz system clock.  Flash read time testing
   // indicates that this provides no speed benefit over operating from a 24.5 MHz clock,
   // so we leave this out and just use the above.
   "c2wd a7 0f",                       // SFRPAGE = 0x0f;  Set SFRPAGE to 0xf
   "c2wd 84 00",                       // CCH0CN = 0x00;   Disable prefetch engine while writing to FLRD bits
   "c2wd a7 00",                       // SFRPAGE = 0x00;  Set SFRPAGE to 0x0
   "c2wd b6 30",                       // FLSCL = 0x30;    Set Flash read timing for 100 MHz
   "c2wd a7 0f",                       // SFRPAGE = 0x0f;  Set SFRPAGE to 0xf
   "c2wd b7 83",                       // OSCICN = 0x83;   Set internal osc to 24.5 MHZ
   "c2wd b3 00",                       // PLL0CN = 0x00;   Select intosc as pll clock source
   "c2wd b3 01",                       // PLL0CN = 0x01;   Enable PLL power
   "c2wd a9 01",                       // PLL0DIV = 0x01;  Divide by 1
   "c2wd a9 01",                       // PLL0DIV = 0x01;  Divide by 1
   "c2wd b1 04",                       // PLL0MUL = 0x04;  Multiply by 4
   "c2wd b2 01",                       // PLL0FLT = 0x01;  ICO = 100 MHz, PD clock 25 MHz
   "wus 5",                            // wait for 5 us
   "c2wd b3 03",                       // PLL0CN = 0x03;   Enable the PLL
   "wms 10",                           // wait for 10 ms for PLL to start
   "c2rd b3",                          // PLL0CN = ??;     read lock value
   "c2wd 8f 04",                       // CLKSEL = 0x04;   Set PLL as system clock
   "c2wd 84 e6",                       // CCH0CN = 0xe6;   0xe7 for block write enable; re-enable prefetch
   "c2wd a7 00",                       // SFRPAGE = 0x00;  Set SFRPAGE to 0x0
*/
   NULL
};

SEGMENT_VARIABLE (T61x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xa0, "C8051T610-GQ", "16K, SAR10",  "QFP32", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xa1, "C8051T611-GM", "16K, SAR10",  "QFN28", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xa2, "C8051T612-GQ", "8K, SAR10",   "QFP32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xa3, "C8051T613-GM", "8K, SAR10",   "QFN28", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xa4, "C8051T614-GQ", "8K, no ADC",  "QFP32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xa5, "C8051T615-GM", "8K, no ADC",  "QFN28", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xa6, "C8051T616-GM", "16K, SAR10",  "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xa7, "C8051T617-GM", "16K, no ADC", "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (T61x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd b2 83",                       // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F336_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xa8, "C8051F336", "16K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xa9, "C8051F337", "16K, no ADC or IDAC", "QFN20", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xaa, "C8051F336", "16K, SAR10, IDAC10",  "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0xab, "C8051F337", "16K, no ADC or IDAC", "QFN24", 0x0000L, 0x3DFFL, 0x3DFFL, 0x3DFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                    NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F336_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd b2 83",                       // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F50x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F500", "64K, SAR12, CAN/LIN",     "QFP/N48", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F501", "64K, SAR12, no CAN/LIN",  "QFP/N48", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F502", "64K, SAR12, CAN/LIN",     "QFP/N32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F503", "64K, SAR12, no CAN/LIN",  "QFP/N32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F504", "32K, SAR12, CAN/LIN",     "QFP/N48", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F505", "32K, SAR12, no CAN/LIN",  "QFP/N48", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F506", "32K, SAR12, CAN/LIN",     "QFP/N32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F507", "32K, SAR12, no CAN/LIN",  "QFP/N32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F508", "64K, SAR12, CAN/LIN",       "QFN40", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F509", "64K, SAR12, no CAN/LIN",    "QFN40", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0xda, "C8051F510", "32K, SAR12, CAN/LIN",       "QFN40", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0xdb, "C8051F511", "32K, SAR12, no CAN/LIN",    "QFN40", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                     NULL,       NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F50x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd ef 00",                     // disable VDD monitor as a reset source
   "c2wd ff a0",                     // set VDD monitor to high threshold
   "wus 100",                        // wait 100 us for VDD monitor to stabilize
   "c2wd ef 02",                     // enable VDD monitor as a reset source
   "c2wd a7 0f",                     // set SFRPAGE to 0x0f
   "c2wd a1 c7",                     // set OSCICN to 24 MHz
   "c2wd 8f 00",                     // set CLKSEL to intosc
   "c2wd a7 00",                     // set SFRPAGE to 0x00
   NULL
};

SEGMENT_VARIABLE (F55x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F550", "32K, SAR12, CAN/LIN",       "QFN24", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F551", "32K, SAR12, CAN",           "QFN24", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F552", "32K, SAR12, LIN",           "QFN24", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F553", "32K, SAR12, no CAN/LIN",    "QFN24", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F554", "16K, SAR12, CAN/LIN",       "QFN24", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F555", "16K, SAR12, CAN",           "QFN24", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F556", "16K, SAR12, LIN",           "QFN24", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F557", "16K, SAR12, no CAN/LIN",    "QFN24", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F560", "32K, SAR12, CAN/LIN",     "QFP/N32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F561", "32K, SAR12, CAN",         "QFP/N32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xda, "C8051F562", "32K, SAR12, LIN",         "QFP/N32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xdb, "C8051F563", "32K, SAR12, no CAN/LIN",  "QFP/N32", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xdc, "C8051F564", "16K, SAR12, CAN/LIN",     "QFP/N32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xdd, "C8051F565", "16K, SAR12, CAN",         "QFP/N32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xde, "C8051F566", "16K, SAR12, LIN",         "QFP/N32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xdf, "C8051F567", "16K, SAR12, no CAN/LIN",  "QFP/N32", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe0, "C8051F568", "32K, SAR12, CAN/LIN",       "QFN40", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xe1, "C8051F569", "32K, SAR12, CAN",           "QFN40", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xe2, "C8051F570", "32K, SAR12, LIN",           "QFN40", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xe3, "C8051F571", "32K, SAR12, no CAN/LIN",    "QFN40", 0x0000L, 0x7BFFL, 0x7BFFL, 0x7BFFL, 0x0000L, 0x00},
   { 0xe4, "C8051F572", "16K, SAR12, CAN/LIN",       "QFN40", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe5, "C8051F573", "16K, SAR12, CAN",           "QFN40", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe6, "C8051F574", "16K, SAR12, LIN",           "QFN40", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe7, "C8051F575", "16K, SAR12, no CAN/LIN",    "QFN40", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe8, "C8051F540", "16K, SAR12, LIN",         "QFP/N32", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xe9, "C8051F541", "16K, SAR12, no LIN",      "QFP/N32", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xea, "C8051F542", "16K, SAR12, LIN",           "QFN24", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xeb, "C8051F543", "16K, SAR12, no LIN",        "QFN24", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xec, "C8051F544",  "8K, SAR12, LIN",         "QFP/N32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xed, "C8051F545",  "8K, SAR12, no LIN",      "QFP/N32", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xee, "C8051F546",  "8K, SAR12, LIN",           "QFN24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xef, "C8051F547",  "8K, SAR12, no LIN",        "QFN24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                 NULL,           NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};
SEGMENT_VARIABLE (F55x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd ef 00",                     // disable VDD monitor as a reset source
   "c2wd ff a0",                     // set VDD monitor to high threshold
   "wus 100",                        // wait 100 us for VDD monitor to stabilize
   "c2wd ef 02",                     // enable VDD monitor as a reset source
   "c2wd a7 0f",                     // set SFRPAGE to 0x0f
   "c2wd a1 c7",                     // set OSCICN to 24 MHz
   "c2wd 8f 00",                     // set CLKSEL to intosc
   "c2wd a7 00",                     // set SFRPAGE to 0x00
   NULL
};

SEGMENT_VARIABLE (F92x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x56, "C8051F930", "64K",  "QFP/N32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0x5e, "C8051F931", "64K",  "QFN24",   0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0xb1, "C8051F920", "32K",  "QFP/N32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0xb3, "C8051F921", "32K",  "QFN24",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0xd0, "Si1000",    "64K",  "QFN42",   0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0xd1, "Si1001",    "32K",  "QFN42",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0xd2, "Si1002",    "64K",  "QFN42",   0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0xd3, "Si1003",    "32K",  "QFN42",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0xd4, "Si1004",    "64K",  "QFN42",   0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0xd5, "Si1005",    "32K",  "QFN42",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0x00, NULL,        NULL,        NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000}
};

SEGMENT_VARIABLE (F92x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd a7 00",                       // set SFRPAGE to 0x00
   "c2wd b2 8F",                       // enable precision oscillator
   "c2wd a9 00",                       // select precision oscillator divide by 1 as system clock source
   NULL
};

SEGMENT_VARIABLE (T63x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xb4, "C8051T630", "8K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x1DFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xb5, "C8051T631", "8K, no ADC or IDAC", "QFN20", 0x0000L, 0x1DFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xb6, "C8051T632", "4K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x0FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xb7, "C8051T633", "4K, no ADC or IDAC", "QFN20", 0x0000L, 0x0FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xb8, "C8051T634", "2K, SAR10, IDAC10",  "QFN20", 0x0000L, 0x07FFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xb9, "C8051T635", "2K, no ADC or IDAC", "QFN20", 0x0000L, 0x07FFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x00, NULL,        NULL,                    NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (T63x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd b2 83",                       // set intosc to 24.5 MHz
   NULL
};

SEGMENT_VARIABLE (F90x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F901",  "8K",  "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0200},
   { 0xd1, "C8051F902",  "8K",  "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0200},
   { 0xd2, "C8051F911", "16K",  "QFN24/QSOP24", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x0200},
   { 0xd3, "C8051F912", "16K",  "QFN24/QSOP24", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x0200},
   { 0x00, NULL,         NULL,            NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000}
};

SEGMENT_VARIABLE (F90x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd a7 00",                       // set SFRPAGE to 0x00
   "c2wd b2 8F",                       // enable precision oscillator
   "c2wd a9 00",                       // select precision oscillator divide by 1 as system clock source
   NULL
};

SEGMENT_VARIABLE (F58x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F580", "128K, SAR12, CAN/LIN",     "QFP/N48", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F581", "128K, SAR12, no CAN/LIN",  "QFP/N48", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F582", "128K, SAR12, CAN/LIN",     "QFP/N32", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F583", "128K, SAR12, no CAN/LIN",  "QFP/N32", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F584", "96K, SAR12, CAN/LIN",      "QFP/N48", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F585", "96K, SAR12, no CAN/LIN",   "QFP/N48", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F586", "96K, SAR12, CAN/LIN",      "QFP/N32", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F587", "96K, SAR12, no CAN/LIN",   "QFP/N32", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F588", "128K, SAR12, CAN/LIN",       "QFN40", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F589", "128K, SAR12, no CAN/LIN",    "QFN40", 0x00000L, 0x1FBFFL, 0x1FBFFL, 0x1FBFFL, 0x0000L, 0x00},
   { 0xda, "C8051F590", "96K, SAR12, CAN/LIN",        "QFN40", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0xdb, "C8051F591", "96K, SAR12, no CAN/LIN",     "QFN40", 0x00000L, 0x17FFFL, 0x17FFFL, 0x17FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                     NULL,        NULL, 0x00000L, 0x00000L, 0x00000L, 0x00000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F58x_INIT[], INIT_STRING, SEG_CODE) =
{ // VDD monitor high; VREG high; INTOSC high; clksel INTOSC
//   "c2wd c9 10",      // set VREG to high value
//   "c2wd ff a0",      // set VDD monitor to high threshold
   "c2wsfr ff a0",	// set VDD monitor to high threshold
   "wus 100",         // wait 100 us for VDD monitor to stabilize
   "c2wsfr ef 02",    // enable VDD monitor as a reset source
   "c2wd a7 0f",      // set SFRPAGE to 0x0F
   "c2wd a1 c7",      // set OSCICN to 0xC7 (24 MHz)
   "c2wd a7 00",      // set SFRPAGE to 0x00
   "c2wd b6 02",      // Extended Flash Erase / Write Timing
   NULL
};


SEGMENT_VARIABLE (F80x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F800", "16K,  SAR10, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F801", "16K,  SAR10, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F802", "16K,  SAR10, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F803", "16K,  SAR10, 512B, 12C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F804", "16K,  SAR10, 512B,  8C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F805", "16K,  SAR10, 512B,  0C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F806", "16K, no ADC, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F807", "16K, no ADC, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F808", "16K, no ADC, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F809", "16K, no ADC, 512B, 12C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xda, "C8051F810", "16K, no ADC, 512B,  8C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xdb, "C8051F811", "16K, no ADC, 512B,  0C",       "SOIC16", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xdc, "C8051F812", " 8K,  SAR10, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xdd, "C8051F813", " 8K,  SAR10, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xde, "C8051F814", " 8K,  SAR10, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xdf, "C8051F815", " 8K,  SAR10, 512B, 12C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe0, "C8051F816", " 8K,  SAR10, 512B,  8C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe1, "C8051F817", " 8K,  SAR10, 512B,  0C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe2, "C8051F818", " 8K, no ADC, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe3, "C8051F819", " 8K, no ADC, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe4, "C8051F820", " 8K, no ADC, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe5, "C8051F821", " 8K, no ADC, 512B, 12C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe6, "C8051F822", " 8K, no ADC, 512B,  8C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe7, "C8051F823", " 8K, no ADC, 512B,  0C",       "SOIC16", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe8, "C8051F824", " 4K,  SAR10, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xe9, "C8051F825", " 4K,  SAR10, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xea, "C8051F826", " 4K,  SAR10, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xeb, "C8051F827", " 4K,  SAR10, 512B, 12C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xec, "C8051F828", " 4K,  SAR10, 512B,  8C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xed, "C8051F829", " 4K,  SAR10, 512B,  0C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xee, "C8051F830", " 4K, no ADC, 512B, 16C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xef, "C8051F831", " 4K, no ADC, 512B,  8C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xf0, "C8051F832", " 4K, no ADC, 512B,  0C", "QSOP24/QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xf1, "C8051F833", " 4K, no ADC, 512B, 12C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xf2, "C8051F834", " 4K, no ADC, 512B,  8C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xf3, "C8051F835", " 4K, no ADC, 512B,  0C",       "SOIC16", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                     NULL,           NULL, 0x00000L, 0x00000L, 0x00000L, 0x00000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F80x_INIT[], INIT_STRING, SEG_CODE) =
{ 
//   "c2wsfr b2 83",                     // set OSCICN to 0x83 (24.5 MHz)
   NULL
};

SEGMENT_VARIABLE (F39x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F390",  "16K,  SAR10, NO EE ",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F391 ", "16K, no ADC, NO EE ",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F392",  "16K,  SAR10, NO EE ",           "QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F393 ", "16K, no ADC, NO EE ",           "QFN20", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F394",  " 8K,  SAR10, NO EE ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F395 ", " 8K, no ADC, NO EE ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F396",  " 8K,  SAR10, NO EE ",           "QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F397 ", " 8K, no ADC, NO EE ",           "QFN20", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F398",  " 4K,  SAR10, NO EE ",           "QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F399 ", " 4K, no ADC, NO EE ",           "QFN20", 0x00000L, 0x00FFFL, 0x00FFFL, 0x00FFFL, 0x0000L, 0x00},
   { 0xda, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xdb, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xdc, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xdd, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xde, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xdf, "*C8051F39x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe0, "C8051F370",  "16K,  SAR10, EE512B",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xe1, "C8051F371 ", "16K, no ADC, EE512B",           "QFN24", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xe2, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe3, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe4, "C8051F374",  " 8K,  SAR10, EE512B",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe5, "C8051F875",  " 8K, no ADC, EE512B",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe6, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe7, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe8, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xe9, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xea, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xeb, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xec, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xed, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xee, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0xef, "*C8051F37x", " undefined         ",           "QFN24", 0x00000L, 0x01FFFL, 0x01FFFL, 0x01FFFL, 0x0000L, 0x00},
   { 0x00,         NULL,                  NULL,              NULL, 0x00000L, 0x00000L, 0x00000L, 0x00000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F39x_INIT[], INIT_STRING, SEG_CODE) =
{ 
   "c2wd ff a0",                     // VDDmon to high threshold
   "c2wd ef 02",                     // enable VDD mon as a reset source
   //"c2wd b6 90",                     // set flash read time to 50 MHz
   "c2wd b2 83",                     // set OSCICN to 0x83 (24.5 MHz)
   NULL
};

SEGMENT_VARIABLE (F70x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F700", "15K, EEPROM, SAR10",   "TQFP64",       0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F701", "15K, EEPROM, No ADC",  "TQFP64",       0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F702", "16K,         SAR10",   "TQFP64",       0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F703", "16K,         No ADC",  "TQFP64",       0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F704", "15K, EEPROM, SAR10",   "TQFP48/QFN48", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F705", "15K, EEPROM, No ADC",  "TQFP48/QFN48", 0x0000L, 0x3BFFL, 0x3BFFL, 0x3BFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F706", "16K,         SAR10",   "TQFP48/QFN48", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F707", "16K,         No ADC",  "TQFP48/QFN48", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F708", " 8K, EEPROM, SAR10",   "TQFP64",       0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F709", " 8K, EEPROM, No ADC",  "TQFP64",       0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xda, "C8051F710", " 8K,         SAR10",   "TQFP64",       0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xdb, "C8051F711", " 8K,         No ADC",  "TQFP64",       0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xdc, "C8051F712", " 8K, EEPROM, SAR10",   "TQFP48/QFN48", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xdd, "C8051F713", " 8K, EEPROM, No ADC",  "TQFP48/QFN48", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xde, "C8051F714", " 8K,         SAR10",   "TQFP48/QFN48", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xdf, "C8051F715", " 8K,         No ADC",  "TQFP48/QFN48", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xe0, "C8051F716", "16K,         SAR10",   "QFN32",        0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0xe1, "C8051F717", "16K,         No ADC",  "QFN24",        0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,  NULL,                  NULL,           0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F70x_INIT[], INIT_STRING, SEG_CODE) =
{ 
   NULL
};


SEGMENT_VARIABLE (F85x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xD0, "C8051F850-GU", "8K, SAR12",  "QSOP24",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xD1, "C8051F851-GU", "4K, SAR12",  "QSOP24",  0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xD2, "C8051F852-GU", "2K, SAR12",  "QSOP24",  0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xD3, "C8051F853-GU", "8K, No ADC", "QSOP24",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xD4, "C8051F854-GU", "4K, No ADC", "QSOP24",  0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xD5, "C8051F855-GU", "2K, No ADC", "QSOP24",  0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xE0, "C8051F860-GS", "8K, SAR12",  "SOIC16",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xE1, "C8051F861-GS", "4K, SAR12",  "SOIC16",  0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xE2, "C8051F862-GS", "2K, SAR12",  "SOIC16",  0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xE3, "C8051F863-GS", "8K, No ADC", "SOIC16",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xE4, "C8051F864-GS", "4K, No ADC", "SOIC16",  0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xE5, "C8051F865-GS", "2K, No ADC", "SOIC16",  0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xF0, "C8051F850-GM", "8K, SAR12",  "QFN20",   0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xF1, "C8051F851-GM", "4K, SAR12",  "QFN20",   0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xF2, "C8051F852-GM", "2K, SAR12",  "QFN20",   0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0xF3, "C8051F853-GM", "8K, No ADC", "QFN20",   0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0xF4, "C8051F854-GM", "4K, No ADC", "QFN20",   0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0xF5, "C8051F855-GM", "2K, No ADC", "QFN20",   0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00,        NULL,     NULL,        NULL,      0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F85x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (F96x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xd0, "C8051F960", "128K, 8K, 128 LCD", "DQFN76/QFP80", 0x00000L, 0x1FFFFL, 0x1FFFFL, 0x1FFFFL, 0x0000L, 0x00},
   { 0xd1, "C8051F961", "128K, 8K,  36 LCD", "QFN40",        0x00000L, 0x1FFFFL, 0x1FFFFL, 0x1FFFFL, 0x0000L, 0x00},
   { 0xd2, "C8051F962", "128K, 8K,  No LCD", "DQFN76/QFP80", 0x00000L, 0x1FFFFL, 0x1FFFFL, 0x1FFFFL, 0x0000L, 0x00},
   { 0xd3, "C8051F963", "128K, 8K,  No LCD", "QFN40",        0x00000L, 0x1FFFFL, 0x1FFFFL, 0x1FFFFL, 0x0000L, 0x00},
   { 0xd4, "C8051F964", " 64K, 8K, 128 LCD", "DQFN76/QFP80", 0x00000L, 0x0FFFFL, 0x0FFFFL, 0x0FFFFL, 0x0000L, 0x00},
   { 0xd5, "C8051F965", " 64K, 8K,  36 LCD", "QFN40",        0x00000L, 0x0FFFFL, 0x0FFFFL, 0x0FFFFL, 0x0000L, 0x00},
   { 0xd6, "C8051F966", " 32K, 8K, 128 LCD", "DQFN76/QFP80", 0x00000L, 0x07FFFL, 0x07FFFL, 0x07FFFL, 0x0000L, 0x00},
   { 0xd7, "C8051F967", " 32K, 8K,  36 LCD", "QFN40",        0x00000L, 0x07FFFL, 0x07FFFL, 0x07FFFL, 0x0000L, 0x00},
   { 0xd8, "C8051F968", " 16K, 4K, 128 LCD", "DQFN76/QFP80", 0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0xd9, "C8051F969", " 16K, 4K,  36 LCD", "QFN40",        0x00000L, 0x03FFFL, 0x03FFFL, 0x03FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                NULL,    NULL,        0x00000L, 0x00000L, 0x00000L, 0x00000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F96x_INIT[], INIT_STRING, SEG_CODE) =
{ 
   "c2wd a7 00",                       // set SFRPAGE to 0x00
   "c2wd ff 88",                       // enable VDD monitor
   "c2wd ef 02",                       // Enable VDDmon as a reset source
   "c2wd a9 04",                       // set CLKSEL to 20 MHz (LP Osc/1)
   "c2wd a7 0f",                       // set SFRPAGE to 0x0F
   "c2wd b6 00",                       // Flash read time One-Shot enabled for system clock < 25 MHz
   "c2wd a7 00",                       // set SFRPAGE to 0x00
   NULL
};

SEGMENT_VARIABLE (F97x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
  { 0xF9, "C8051F970-GM", "32 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-48-A", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0xFA, "C8051F971-GM", "32 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-32-A", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0xFB, "C8051F972-GM", "32 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-24-A", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0xFC, "C8051F973-GM", "16 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-48-A", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0xFD, "C8051F974-GM", "16 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-32-A", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0xFE, "C8051F975-GM", "16 kB Flash, 8 kB RAM, 12-bit ADC, Capacitive Sensing", "QFN-24-A", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x00, NULL, NULL, NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (F97x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd a7 00",                     // set SFRPAGE to 0x00
   "c2wd a9 04",                     // set CLKSEL to 20 MHz (LP Osc/1)
   NULL
};

SEGMENT_VARIABLE (F99x_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0xD0, "C8051F990", "8K, SAR12",  "QFN20",        0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD1, "C8051F991", "8K, No ADC", "QFN20",        0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD6, "C8051F996", "8K, SAR12",  "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD2, "C8051F997", "8K, No ADC", "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD3, "C8051F980", "8K, SAR12",  "QFN20",        0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD4, "C8051F981", "8K, No ADC", "QFN20",        0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xD5, "C8051F982", "4K, SAR10",  "QFN20",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x0400},
   { 0xD7, "C8051F983", "4K, No ADC", "QFN20",        0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x0400},
   { 0xD8, "C8051F985", "2K, No ADC", "QFN20",        0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x0400},
   { 0xD9, "C8051F986", "8K, SAR12",  "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xDA, "C8051F987", "8K, No ADC", "QFN24/QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0xDB, "C8051F988", "4K, SAR12",  "QFN24/QSOP24", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x0400},
   { 0xDC, "C8051F989", "4K, No ADC", "QFN24/QSOP24", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x0400},
   { 0x00, NULL,        NULL,          NULL,          0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000}
};

SEGMENT_VARIABLE (F99x_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd ff 80",                       // enable VDD monitor
   "c2wd ef 02",                       // Enable VDDmon as a reset source
   "c2wd a9 04",                       // set CLKSEL to 20 MHz (LP Osc/1)
   "c2wd b6 40",                       // Flash read timing one-shot bypass for system clock greater than 14 MHz
   NULL
};

SEGMENT_VARIABLE (BB1_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x01, "EFM8BB10F8G-QSOP24", "8K, 512 B RAM",  "QSOP24",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x02, "EFM8BB10F8G-QFN20", "8K, 512 B RAM",  "QFN20",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x03, "EFM8BB10F8G-SOIC16", "8K, 512 B RAM",  "SOIC16",  0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x00},
   { 0x05, "EFM8BB10F4G-QFN20", "4K, 512 B RAM", "QFN20",  0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x00},
   { 0x08, "EFM8BB10F2G-QFN20", "2K, 256 B RAM", "QFN20",  0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x00},
   { 0x00,        NULL,     NULL,        NULL,      0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (BB1_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (BB2_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x01, "EFM8BB22F16G-QFN28", "16K, 2304 B RAM, SAR12, VREG",  "QFN28",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x400},
   { 0x03, "EFM8BB21F16G-QFN20", "16K, 2304 B RAM, SAR12",  "QFN20",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x400},
   { 0x02, "EFM8BB21F16G-QSOP24", "16K, 2304 B RAM, SAR12",  "QSOP24",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x400},
   { 0x00,        NULL,     NULL,        NULL,      0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (BB2_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (BB3_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
  { 0x01, "EFM8BB31F64G-QFN32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x02, "EFM8BB31F64G-QFP32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFP32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x03, "EFM8BB31F64G-QSOP24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QSOP24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x04, "EFM8BB31F64G-QFN24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x05, "EFM8BB31F32G-QFN32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x06, "EFM8BB31F32G-QFP32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x07, "EFM8BB31F32G-QSOP24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x08, "EFM8BB31F32G-QFN24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x09, "EFM8BB31F16G-QFN32", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x0A, "EFM8BB31F16G-QFP32", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x0B, "EFM8BB31F16G-QSOP24", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x0C, "EFM8BB31F16G-QFN24", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x11, "EFM8BB31F64I-QFN32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x12, "EFM8BB31F64I-QFP32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFP32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x13, "EFM8BB31F64I-QSOP24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QSOP24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x14, "EFM8BB31F64I-QFN24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x15, "EFM8BB31F32I-QFN32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x16, "EFM8BB31F32I-QFP32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x17, "EFM8BB31F32I-QSOP24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x18, "EFM8BB31F32I-QFN24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x19, "EFM8BB31F16I-QFN32", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x1A, "EFM8BB31F16I-QFP32", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x1B, "EFM8BB31F16I-QSOP24", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x1C, "EFM8BB31F16I-QFN24", "16 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x00, NULL, NULL, NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (BB3_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (LB1_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
  { 0x41, "EFM8LB12F64E-QFN32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x42, "EFM8LB12F64E-QFP32", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFP32", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x43, "EFM8LB12F64E-QSOP24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QSOP24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x44, "EFM8LB12F64E-QFN24", "64 KB Flash, 4352 B RAM, ADC, 4 DACs", "QFN24", 0x0000, 0xFC00, 0xFBFF, 0xFBFF, 0x0000, 0x00,},
  { 0x45, "EFM8LB12F32E-QFN32", "32 KB Flash, 2304 B RAM, ADC, 4 DACs", "QFN32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x46, "EFM8LB12F32E-QFP32", "32 KB Flash, 2304 B RAM, ADC, 4 DACs", "QFP32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x47, "EFM8LB12F32E-QSOP24", "32 KB Flash, 2304 B RAM, ADC, 4 DACs", "QSOP24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x48, "EFM8LB12F32E-QFN24", "32 KB Flash, 2304 B RAM, ADC, 4 DACs", "QFN24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x49, "EFM8LB11F32E-QFN32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x4A, "EFM8LB11F32E-QFP32", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x4B, "EFM8LB11F32E-QSOP24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x4C, "EFM8LB11F32E-QFN24", "32 KB Flash, 2304 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x8000, 0x7FFF, 0x7FFF, 0x0000, 0x00,},
  { 0x4D, "EFM8LB11F16E-QFN32", "16 KB Flash, 1280 B RAM, ADC, 2 DACs", "QFN32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x4E, "EFM8LB11F16E-QFP32", "16 KB Flash, 1280 B RAM, ADC, 2 DACs", "QFP32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x4F, "EFM8LB11F16E-QSOP24", "16 KB Flash, 1280 B RAM, ADC, 2 DACs", "QSOP24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x50, "EFM8LB11F16E-QFN24", "16 KB Flash, 1280 B RAM, ADC, 2 DACs", "QFN24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x51, "EFM8LB10F16E-QFN32", "16 KB Flash, 1280 B RAM, ADC, 0 DACs", "QFN32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x52, "EFM8LB10F16E-QFP32", "16 KB Flash, 1280 B RAM, ADC, 0 DACs", "QFP32", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x53, "EFM8LB10F16E-QSOP24", "16 KB Flash, 1280 B RAM, ADC, 0 DACs", "QSOP24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x54, "EFM8LB10F16E-QFN24", "16 KB Flash, 1280 B RAM, ADC, 0 DACs", "QFN24", 0x0000, 0x4000, 0x3FFF, 0x3FFF, 0x0000, 0x00,},
  { 0x00, NULL, NULL, NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (LB1_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (SB1_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x01, "EFM8SB10F8G-QFN24", "8K, 512 B RAM",  "QFN24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0x02, "EFM8SB10F8G-QSOP24", "8K, 512 B RAM", "QSOP24", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0x03, "EFM8SB10F8G-QFN20", "8K, 512 B RAM",  "QFN20", 0x0000L, 0x1FFFL, 0x1FFFL, 0x1FFFL, 0x0000L, 0x0400},
   { 0x06, "EFM8SB10F4G-QFN20", "4K, 512 B RAM", "QFN20", 0x0000L, 0x0FFFL, 0x0FFFL, 0x0FFFL, 0x0000L, 0x0400},
   { 0x09, "EFM8SB10F2G-QFN20", "2K, 256 B RAM",  "QFN20", 0x0000L, 0x07FFL, 0x07FFL, 0x07FFL, 0x0000L, 0x0400},
   { 0x00, NULL,        NULL,          NULL,          0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000}
};

SEGMENT_VARIABLE (SB1_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd ff 80",                       // enable VDD monitor
   "c2wd ef 02",                       // Enable VDDmon as a reset source
   "c2wd a9 04",                       // set CLKSEL to 20 MHz (LP Osc/1)
   "c2wd b6 40",                       // Flash read timing one-shot bypass for system clock greater than 14 MHz
   NULL
};

SEGMENT_VARIABLE (SB2_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x01, "EFM8SB20F64G-QFN32", "64 KB Flash, 4 KB RAM",  "QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0x02, "EFM8SB20F64G-QFP32", "64 KB Flash, 4 KB RAM",  "QFP32",   0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0x03, "EFM8SB20F64G-QFN24", "64 KB Flash, 4 KB RAM",  "QFN24", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x0400},
   { 0x04, "EFM8SB20F32G-QFN32", "32 KB Flash, 4 KB RAM",  "QFN32",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0x05, "EFM8SB20F32G-QFP32", "32 KB Flash, 4 KB RAM",  "QFP32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0x06, "EFM8SB20F32G-QFN24", "32 KB Flash, 4 KB RAM",  "QFN24",   0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x0400},
   { 0x09, "EFM8SB20F16G-QFN24", "16 KB Flash, 4 KB RAM",  "QFN24", 0x0000L, 0x3FFFL, 0x3FFFL, 0x3FFFL, 0x0000L, 0x0400},
   { 0x00, NULL,        NULL,        NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000}
};

SEGMENT_VARIABLE (SB2_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wd a7 00",                       // set SFRPAGE to 0x00
   "c2wd b2 8F",                       // enable precision oscillator
   "c2wd a9 00",                       // select precision oscillator divide by 1 as system clock source
   NULL
};

SEGMENT_VARIABLE (UB1_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x41, "EFM8UB10F16G-QFN28", "16K, 2304 B RAM, SAR12, VREG",  "QFN28",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x0400},
   { 0x43, "EFM8UB10F16G-QFN20", "16K, 2304 B RAM, SAR12",  "QFN20",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x0400},
   { 0x45, "EFM8UB11F16G-QSOP24", "16K, 2304 B RAM, SAR12",  "QSOP24",  0x0000L, 0x4000L, 0xFBFF, 0xFBFF, 0xF800L, 0x0400},
   { 0x49, "EFM8UB10F8G-QFN20", "16K, 2304 B RAM, SAR12",  "QFN20",  0x0000L, 0x2000L, 0xFBFF, 0xFBFF, 0xF800L, 0x0400},
   { 0x00,        NULL,     NULL,        NULL,      0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (UB1_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr ff 80",                     // Enable VDD monitor
   "wus 5",                            // wait 5 us for VDD monitor to stabilize
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 00",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

SEGMENT_VARIABLE (UB2_FAMILY[], DERIVATIVE_ENTRY, SEG_CODE) =
{
   { 0x60, "EFM8UB20F64G-QFP48", "64 KB Flash, 4 KB RAM", "QFP48", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x61, "EFM8UB20F64G-QFP32", "64 KB Flash, 4 KB RAM", "QFP32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x62, "EFM8UB20F64G-QFN32", "64 KB Flash, 4 KB RAM", "QFN32", 0x0000L, 0xFBFFL, 0xFBFFL, 0xFBFFL, 0x0000L, 0x00},
   { 0x63, "EFM8UB20F32G-QFP48", "32 KB Flash, 2 KB RAM", "QFP48", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x64, "EFM8UB20F32G-QFP32", "32 KB Flash, 2 KB RAM", "QFP32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x65, "EFM8UB20F32G-QFN32", "32 KB Flash, 2 KB RAM", "QFN32", 0x0000L, 0x7FFFL, 0x7FFFL, 0x7FFFL, 0x0000L, 0x00},
   { 0x00,        NULL,                   NULL,           NULL, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x0000L, 0x00}
};

SEGMENT_VARIABLE (UB2_INIT[], INIT_STRING, SEG_CODE) =
{
   "c2wsfr b6 90",                     // Set Flash read timing for 48 MHz, one shot enabled
   "c2wsfr ff 80",                     // Enable VDD monitor
   "c2wsfr ef 02",                     // Enable VDDmon as a reset source
   "c2wsfr a9 03",                     // Set Internal High-Frequency Oscillator as system clock
   NULL
};

// DEVICE_ID                     MEM_TYPE  PAGE_SIZE   SECURITY_TYPE    INIT
//        FAMILY_STRING                          HAS_SFLE       FPDAT   	       DERIVATIVE list

//DEVICE_FAMILY code KNOWN_FAMILIES[] =
SEGMENT_VARIABLE (KNOWN_FAMILIES[], DEVICE_FAMILY, SEG_CODE) =
{
   { 0x04, "C8051F30x",             FLASH,  512, FALSE,  C2_1,   0xB4,  F30x_INIT,  F30x_FAMILY },
   { 0x08, "C8051F31x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F31x_INIT,  F31x_FAMILY },
   { 0x09, "C8051F32x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F32x_INIT,  F32x_FAMILY },
   { 0x0a, "C8051F33x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F33x_INIT,  F33x_FAMILY },
   { 0x0b, "C8051F35x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F35x_INIT,  F35x_FAMILY },
   { 0x0c, "C8051F41x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F41x_INIT,  F41x_FAMILY },
   { 0x0d, "C8051F326/7",           FLASH,  512, FALSE,  C2_2,   0xB4,  F326_INIT,  F326_FAMILY },
   { 0x0e, "Si825x",                FLASH,  512, FALSE,  C2_2,   0xB4, Si8250_INIT, Si8250_FAMILY },
   { 0x0f, "C8051F34x",             FLASH,  512, FALSE,  C2_2,   0xAD,  F34x_INIT,  F34x_FAMILY },
   { 0x10, "C8051T60x",               OTP,  512, FALSE,  C2_2,   0xB4,  T60x_INIT,  T60x_FAMILY },
   { 0x11, "C8051F52x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F52x_INIT,  F52x_FAMILY },
   { 0x12, "C8051F36x",             FLASH, 1024, FALSE,  C2_2,   0xB4,  F36x_INIT,  F36x_FAMILY },
   { 0x13, "C8051T61x",               OTP,  512, FALSE,  C2_2,   0xB4,  T61x_INIT,  T61x_FAMILY },
   { 0x14, "C8051F336/7",           FLASH,  512, FALSE,  C2_2,   0xB4,  F336_INIT,  F336_FAMILY },
   { 0x15, "Si8100",                FLASH,  512, FALSE,  C2_2,   0xB4, DUMMY_INIT, DUMMY_FAMILY },
   { 0x16, "C8051F92x/93x",         FLASH, 1024, FALSE,  C2_2,   0xB4,  F92x_INIT,  F92x_FAMILY },
   { 0x17, "C8051T63x",               OTP,  512, FALSE,  C2_2,   0xB4,  T63x_INIT,  T63x_FAMILY },
   { 0x18, "C8051T62x",               OTP,  512, FALSE,  C2_2,   0xAD, DUMMY_INIT, DUMMY_FAMILY },
   { 0x19, "C8051T622/3",             OTP,  512, FALSE,  C2_2,   0xAD, DUMMY_INIT, DUMMY_FAMILY },
   { 0x1a, "C8051T624/5",             OTP,  512, FALSE,  C2_2,   0xAD, DUMMY_INIT, DUMMY_FAMILY },
   { 0x1b, "C8051T606",               OTP,  512, FALSE,  C2_2,   0xB4, DUMMY_INIT, DUMMY_FAMILY },
   { 0x1c, "C8051F50x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F50x_INIT,  F50x_FAMILY },
   { 0x1d, "C8051F338POE",          FLASH,  512, FALSE,  C2_2,   0xB4, DUMMY_INIT, DUMMY_FAMILY },
   { 0x1e, "C8051F70x/71x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F70x_INIT,  F70x_FAMILY },
   { 0x1f, "C8051F90x/91x",         FLASH,  512,  TRUE,  C2_2,   0xB4,  F90x_INIT,  F90x_FAMILY },
   { 0x20, "C8051F58x/59x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F58x_INIT,  F58x_FAMILY },
   { 0x21, "C8051F54x",             FLASH,  512, FALSE,  C2_2,   0xB4, DUMMY_INIT, DUMMY_FAMILY },
   { 0x22, "C8051F54x/55x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F55x_INIT,  F55x_FAMILY },
   { 0x23, "C8051F80x/81x/82x/83x", FLASH,  512, FALSE,  C2_2,   0xB4,  F80x_INIT,  F80x_FAMILY },
   { 0x25, "C8051F99x/98x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F99x_INIT,  F99x_FAMILY },
   { 0x28, "C8051F38x",             FLASH,  512, FALSE,  C2_2,   0xAD,  F38x_INIT,  F38x_FAMILY },
   { 0x2a, "C8051F960",             FLASH, 1024, FALSE,  C2_2,   0xB4,  F96x_INIT,  F96x_FAMILY },
   { 0x2b, "C8051F39x/37x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F39x_INIT,  F39x_FAMILY },
   { 0x30, "C8051F85x/86x",         FLASH,  512, FALSE,  C2_2,   0xB4,  F85x_INIT,  F85x_FAMILY },
   { 0x29, "C8051F97x",             FLASH,  512, FALSE,  C2_2,   0xB4,  F97x_INIT,  F97x_FAMILY },
   
   { 0x30, "EFM8BB1",               FLASH,  512, FALSE,  C2_2,   0xB4,  BB1_INIT,   BB1_FAMILY },
   { 0x32, "EFM8BB2",               FLASH,  512, FALSE,  C2_2,   0xB4,  BB2_INIT,   BB2_FAMILY },
   { 0x34, "EFM8BB3",               FLASH,  512, FALSE,  C2_2,   0xB4,  BB3_INIT,   BB3_FAMILY },
   { 0x34, "EFM8LB1",               FLASH,  512, FALSE,  C2_2,   0xB4,  LB1_INIT,   LB1_FAMILY },
   { 0x25, "EFM8SB1",               FLASH,  512, FALSE,  C2_2,   0xB4,  SB1_INIT,   SB1_FAMILY },
   { 0x16, "EFM8SB2",               FLASH, 1024, FALSE,  C2_2,   0xB4,  SB2_INIT,   SB2_FAMILY },
   { 0x32, "EFM8UB1",               FLASH,  512, FALSE,  C2_2,   0xB4,  UB1_INIT,   UB1_FAMILY },
   { 0x28, "EFM8UB2",               FLASH,  512, FALSE,  C2_2,   0xAD,  UB2_INIT,   UB2_FAMILY },
   
   { 0x00,        NULL,             FALSE, FALSE, FALSE, FALSE, FALSE, DUMMY_INIT, DUMMY_FAMILY }
};


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

