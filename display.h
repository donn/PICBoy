/* 
 * File:   		display.h
 * Author: 		Muhammad N ElNokrashy
 * Comments:	Adapted from Adafruit graphics library (Adafruit_GFX.h)
 * 				to target the ST7735 on PIC32 boards.
 * Revision history: 
 */


#define swap(a, b) do {     \
    __auto_type temp = a;   \
    a = b;                  \
    b = temp;               \
} while(0)

#if !defined(NAMESPACE)
#define NAMESPACE pboy
#endif

#define CONCAT(a, b) a ## b
#define NS(x) CONCAT(CONCAT(NAMESPACE, _), x)

#define ONES(n) (~(~0 << n))

typedef unsigned short  u16;
typedef unsigned char   u8;
typedef signed short    i16;
typedef signed char     i8;

typedef u8 Gray;

u8 NS(width), NS(height);

void
NS(drawGrayscale)(i16 x, i16 y, const u8* img, i16 w, i16 h);

