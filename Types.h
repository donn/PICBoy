#ifndef _types_h
#define _types_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define bit uint8_t// I'm already sorry
#define nibble uint8_t //I'M REALLY SORRY
#define byte uint8_t
#define word uint16_t
#define uint32 uint32_t
#define displacement int8_t

#define SET_BIT(i, j) do { i = i | (1 << j); } while (0)
#define RESET_BIT(i, j) do { i = i & ~(1 << j); } while (0)
#define TEST_BIT(i, j)  ((i >> j) & 1)

#define SWAP_NIBBLES(i) do {\
    i = (i >> 4) | (i << 8);\
} while(0);

#define SIGN_EXTEND(i) (uint16_t)(int16_t)(int8_t)(uint8_t)(i)

#define C_FLAG 4
#define H_FLAG 5
#define N_FLAG 6
#define Z_FLAG 7


#endif
