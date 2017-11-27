#ifndef _types_h
#define _types_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define bit unsigned char // I'm already sorry
#define byte unsigned char
#define half unsigned char //I'M REALLY SORRY
#define word unsigned short
#define uint32 uint32_t
#define displacement char

#define SET_BIT(i, j) do { i = i | (1 << j); } while (0)
#define RESET_BIT(i, j) do { i = i & ~(1 << j); } while (0)
#define TEST_BIT(i, j)  ((i >> j) & 1)

#define C_FLAG 4
#define H_FLAG 5
#define N_FLAG 6
#define Z_FLAG 7


#endif
