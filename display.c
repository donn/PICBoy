/* 
 * File:   		display.c
 * Author: 		Muhammad N ElNokrashy
 * Comments:	Adapted from Adafruit graphics library (Adafruit_GFX.h)
 * 				to target the ST7735 on PIC32 boards.
 * Revision history: 
 */

#include <plib.h>
#include <xc.h>

#define _hmDISPLAY_C 1
#include "display.h"
#include "display_commands.h"

#define XFREQ 80000000

#include "common.h"

#define CS (LATDbits.LATD3)
#define DC (LATDbits.LATD10)

#define DELAY 1

static const byte
	Bcmd[] = {                  	// Initialization commands for 7735B screens
		18,                       	// 18 commands in list:
		ST7735_SWRESET,   DELAY,  	//  1: Software reset, no args, w/delay
		50,                     	//     50 ms delay
		ST7735_SLPOUT ,   DELAY,  	//  2: Out of sleep mode, no args, w/delay
		255,                    	//     255 = 500 ms delay
		ST7735_COLMOD , 1+DELAY,  	//  3: Set color mode, 1 arg + delay:
		0x05,                   	//     16-bit color
		10,                     	//     10 ms delay
		ST7735_FRMCTR1, 3+DELAY,  	//  4: Frame rate control, 3 args + delay:
		0x00,                   	//     fastest refresh
		0x06,                   	//     6 lines front porch
		0x03,                   	//     3 lines back porch
		10,                     	//     10 ms delay
		ST7735_MADCTL , 1      ,  	//  5: Memory access ctrl (directions), 1 arg:
		0x08,                   	//     Row addr/col addr, bottom to top refresh
		ST7735_DISSET5, 2      ,  	//  6: Display settings #5, 2 args, no delay:
		0x15,                   	//     1 clk cycle nonoverlap, 2 cycle gate
									//     rise, 3 cycle osc equalize
		0x02,                   	//     Fix on VTL
		ST7735_INVCTR , 1      ,  	//  7: Display inversion control, 1 arg:
		0x0,                    	//     Line inversion
		ST7735_PWCTR1 , 2+DELAY,  	//  8: Power control, 2 args + delay:
		0x02,                   	//     GVDD = 4.7V
		0x70,                   	//     1.0uA
		10,                     	//     10 ms delay
		ST7735_PWCTR2 , 1      ,  	//  9: Power control, 1 arg, no delay:
		0x05,                   	//     VGH = 14.7V, VGL = -7.35V
		ST7735_PWCTR3 , 2      ,  	// 10: Power control, 2 args, no delay:
		0x01,                   	//     Opamp current small
		0x02,                   	//     Boost frequency
		ST7735_VMCTR1 , 2+DELAY,  	// 11: Power control, 2 args + delay:
		0x3C,                   	//     VCOMH = 4V
		0x38,                   	//     VCOML = -1.1V
		10,                     	//     10 ms delay
		ST7735_PWCTR6 , 2      ,  	// 12: Power control, 2 args, no delay:
		0x11, 0x15,
		ST7735_GMCTRP1,16      ,  	// 13: Magical unicorn dust, 16 args, no delay:
		0x09, 0x16, 0x09, 0x20, 	//     (seriously though, not sure what
		0x21, 0x1B, 0x13, 0x19, 	//      these config values represent)
		0x17, 0x15, 0x1E, 0x2B,
		0x04, 0x05, 0x02, 0x0E,
		ST7735_GMCTRN1,16+DELAY,  	// 14: Sparkles and rainbows, 16 args + delay:
		0x0B, 0x14, 0x08, 0x1E, 	//     (ditto)
		0x22, 0x1D, 0x18, 0x1E,
		0x1B, 0x1A, 0x24, 0x2B,
		0x06, 0x06, 0x02, 0x0F,
		10,                     	//     10 ms delay
		ST7735_CASET  , 4      ,  	// 15: Column addr set, 4 args, no delay:
		0x00, 0x02,             	//     XSTART = 2
		0x00, 0x81,             	//     XEND = 129
		ST7735_RASET  , 4      ,  	// 16: Row addr set, 4 args, no delay:
		0x00, 0x02,             	//     XSTART = 1
		0x00, 0x81,             	//     XEND = 160
		ST7735_NORON  ,   DELAY,  	// 17: Normal display on, no args, w/delay
		10,                     	//     10 ms delay
		ST7735_DISPON ,   DELAY,  	// 18: Main screen turn on, no args, w/delay
		255 }                   	//     255 = 500 ms delay
;

static const byte
    NS(width) 	= 160,
    NS(height) 	= 128;

static
uint16
swap_color(uint16 c) {
	// BGR -> RGB
	// RGB -> BGR
	return (c << 11) | (c & (ONES(6) << 5)) | (c >> 11);
}

/// Takes full 256-level color
static
uint16
_grayColor(byte lvl) {
	byte x = (lvl >> 3);

	// returns 5-6-5 -based gray (more greenish than normal)
	return (x << 11) | (x << 5) | (x);
}

inline static
void
_spi_write(byte b) { WriteSPI1(b); }


inline static
void
_spi_write16(uint16 word) {
	_spi_write(word >> 8);
	_spi_write(word & 0xFF);
}

static inline
void
_writeData(byte b) {
	_dc_hi();
	_cs_lo();

	_spi_write(b);

	_cs_hi();
}

static inline
void
_writeCommand(byte b) {
	_cs_lo();
	_dc_lo();

	// spi write
	_spi_write(b);

	_cs_hi();
}

static inline
void
_writeCommands(const byte *restrict cmd) {
	uint8_t nCmds = *(cmd++), nArgs;
	uint16 ms;

	while(nCmds--) {
		_writeCommand(*(cmd++));
		nArgs  = *(cmd++);
		ms     = nArgs & DELAY;
		nArgs &= ~DELAY;
		while(nArgs--) {
			_writeData(*(cmd++));
		}

		if(ms) {
			ms = *(cmd++);
			if(ms == 255) ms = 500;
			delay_ms(ms);
		}
	}
}


void
NS(init)() {
	_cs_lo();
	_writeCommands(Bcmd);
}

void
_setWindow(byte x0, byte y0, byte x1, byte y1) {
	_writeCommand(ST7735_CASET);
	_writeData(0x00);
	_writeData(x0);     // XSTART 
	_writeData(0x00);
	_writeData(x1);     // XEND

	_writeCommand(ST7735_RASET); // Row addr set
	_writeData(0x00);
	_writeData(y0);     // YSTART
	_writeData(0x00);
	_writeData(y1);     // YEND

	_writeCommand(ST7735_RAMWR); // write to RAM
}

void
NS(drawPixel)(byte x, byte y, uint16 color) {
	if((x < 0) || (y < 0)
		|| (x >= NS(width)) || (y >= NS(height)))
		return;

	_setWindow(x, y, x+1, y+1);

	_dc_hi();
	_cs_lo();
	
	_spi_write16(color);

	_cs_hi();
}

void
NS(fillRect)(int16 x0, int16 y0, byte color, int16 w, int16 h) {
	// i : h
	// j : w
	
	_setWindow(x0, y0, x0+w-1, y0+h-1);

	_dc_hi();
	_cs_lo();

	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			_spi_write16(_grayColor(color));
			// NS(drawPixel)(x0 + j, y0 + i, color);
		}
	}

	_cs_hi();
}

void
NS(fillScreen)(byte color) {
	NS(fillRect)(0, 0, color, NS(width), NS(height));
}

void
NS(drawGrayBitmap)(int16 x0, int16 y0, const byte *restrict bitmap, int16 w, int16 h) {
	// i : h
	// j : w
	
	_setWindow(x0, y0, x0+w-1, y0+h-1);

	_dc_hi();
	_cs_lo();

	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			uint16 color = _grayColor(*(bitmap + i*w + j));
			_spi_write16(color);
			// NS(drawPixel)(x0 + j, y0 + i, color);
		}
	}

	_cs_hi();
}


#pragma region "SPI bit helpers"
inline static
void
_cs_hi() {
	LATDbits.LATD9 = 1;
}

inline static
void
_cs_lo() {
	LATDbits.LATD9 = 0;
}

inline static
void
_dc_hi() {
	LATDbits.LATD8 = 1;
}

inline static
void
_dc_lo() {
	LATDbits.LATD8 = 0;
}
#pragma endregion