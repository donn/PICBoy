#ifndef _hTYPES_H
#define _hTYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t     bit;    // G: I'm already sorry
                            // N: AS YOU SHOULD BE
typedef uint8_t     nibble; // I'M REALLY SORRY
typedef uint8_t     byte;
typedef  int8_t     sbyte;
typedef uint16_t    word;
typedef uint16_t    uint16;
typedef  int16_t     int16;
typedef uint32_t    uint32;
typedef  int32_t     int32;
typedef int8_t      displacement;


#define SET_BIT(n, i)   do { n |= (1 << i); } while (0)
#define RESET_BIT(n, i) do { n &= ~(1 << i); } while (0)
#define TEST_BIT(n, i)     ((n >> i) & 1)

// !FIXME! should be `i = ((i>>4)&0xF) | (i<<4)`?
#define SWAP_NIBBLES(n) do {\
    n = (n >> 4) | (n << 8);\
    /*n = ((n>>4)&0xF) | (n<<4);*/ \
} while(0);

// !DISCUSS! *blink*
#define SIGN_EXTEND(i) ((uint16_t)(int16_t)(int8_t)(uint8_t)(i))
// static inline uint16_t extend_sign(uint8_t n) {
//     return ((uint16_t)(n < 0 ? ~0 : 0) & n);
//     // return (((uint16_t)(~0) * (n < 0)) & n);
//     // return (uint16_t)(((int16_t)(n) << 8) >> 8);
// }

#define C_FLAG 4
#define H_FLAG 5
#define N_FLAG 6
#define Z_FLAG 7

#define forever for(;;)

#endif // _hTYPES_H
