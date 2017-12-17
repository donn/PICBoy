/* 
 * File:   		display.c
 * Author: 		Muhammad N ElNokrashy
 * Comments:	Adapted from Adafruit graphics library (Adafruit_GFX.h)
 * 				to target the ST7735 on PIC32 boards.
 * Revision history: 
 */

#include <plib.h>
#include "display.h"

static
u16
swap_color(u16 c) {
	// BGR -> RGB
	// RGB -> BGR
	return (c << 11) | (c & (ONES(6) << 5)) | (c >> 11);
}

/// Takes full 256-level color
static
u16
_grayColor(u8 lvl) {
	u8 x = (lvl >> 3);

	// returns 5-6-5 -based gray (more greenish than normal)
	return (x << 11) | (x << 5) | (x);
}

inline static
void
_cs_hi() {

}

inline static
void
_cs_lo() {

}

inline static
void
_dc_hi() {

}

inline static
void
_dc_lo() {

}


inline static
void
_spi_write16(u16 hword) {
	// wait on buffer space
	while(_TxBuffSPI1()) {}

	// Actually write hword
	_writeSPI1(hword);

	while(SPI1STATbits.SPIBUSY) {}
}


void
NS(drawPixel)(u8 x, u8 y, u16 color) {
	if((x < 0) || (y < 0)
		|| (x >= NS(width)) || (y >= NS(height)))
		return;

	// Set window address

	_dc_hi();
	_cs_lo();
	
	_spi_write16(color);

	_cs_hi();
}

void
NS(drawGrayscale)(i16 x, i16 y, const u8* img, i16 w, i16 h) {
	// i : h
	// j : w
	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			u16 color = _grayColor(*(img + i*w + j));
			NS(drawPixel)(x + j, y + i, color);
		}
	}
}