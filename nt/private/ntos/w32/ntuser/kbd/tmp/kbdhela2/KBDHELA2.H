/****************************** Module Header ******************************\
* Module Name: KBDHELA2.H
*
* keyboard layout header for Greek IBM 220 Latin
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Various defines for use by keyboard input code.
*
* History:
*
* created by KBDTOOL Thu Oct 17 16:38:13 2019
*
\***************************************************************************/

/*
 * kbd type should be controlled by cl command-line argument
#define KBD_TYPE 4

/*
* Include the basis of all keyboard table values
*/
#include "kbd.h"

/***************************************************************************\
* The table below defines the virtual keys for various keyboard types where
* the keyboard differ from the US keyboard.
*
* _EQ() : all keyboard types have the same virtual key for this scancode
* _NE() : different virtual keys for this scancode, depending on kbd type
*
*     +------+ +----------+----------+----------+----------+----------+----------+
*     | Scan | |    kbd   |    kbd   |    kbd   |    kbd   |    kbd   |    kbd   |
*     | code | |   type 1 |   type 2 |   type 3 |   type 4 |   type 5 |   type 6 |
\****+-------+_+----------+----------+----------+----------+----------+----------+*/

#undef  T0C
#define T0C _EQ(                                      OEM_2                      )
#undef  T0D
#define T0D _EQ(                                      OEM_4                      )
#undef  T1A
#define T1A _EQ(                                   OEM_PLUS                      )
#undef  T35
#define T35 _EQ(                                  OEM_MINUS                      )

