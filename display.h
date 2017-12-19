/* 
 * File:   		display.h
 * Author: 		Muhammad N ElNokrashy
 * Comments:	Adapted from Adafruit graphics library (Adafruit_GFX.h)
 * 				to target the ST7735 on PIC32 boards.
 * Revision history: 
 */

#ifndef _hDISPLAY_H
#define _hDISPLAY_H

#include "Types.h"

#define swap(a, b) do {     \
    __auto_type temp = a;   \
    a = b;                  \
    b = temp;               \
} while(0)

#define _CONCAT(x, y) x##y
#define CONCAT(a, b) _CONCAT(a, b)

#if !defined(NAMESPACE)
#define NAMESPACE pboy
#endif

#define NS(x) CONCAT(CONCAT(NAMESPACE, _), x)

#define ONES(n) (~(~0u << n))


typedef enum _ScreenDirection {
    sdUp=0, sdRight, sdDown, sdLeft
} ScreenDirection;

typedef byte Gray;

static const byte
    NS(width),
    NS(height);

void
NS(init)();

void
NS(drawPixel)(byte x, byte y, uint16 color);

void
NS(drawGrayBitmap)(int16 x0, int16 y0, const byte *restrict bitmap, int16 w, int16 h);

void
NS(fillRect)(int16 x0, int16 y0, byte color, int16 w, int16 h);

void
NS(fillScreen)(byte color);


#if !_hmDISPLAY_C
#undef NAMESPACE
#undef NS
#else
#pragma region "SPI bit helpers"
inline static
void
_cs_hi();

inline static
void
_cs_lo();

inline static
void
_dc_hi();

inline static
void
_dc_lo();
#pragma endregion
#endif // _hmDISPLAY_C

#endif // _hDISPLAY_H