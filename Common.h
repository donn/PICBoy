#ifndef _types_h
#define _types_h

#if defined(__APPLE__) || defined(__unix__)
    #define _posix 1
#endif

#if _posix
    #include <stdio.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define bit uint8_t // Sorry.
#define nibble uint8_t // Sorry again (semantics are everything)
#define byte uint8_t
#define displacement int8_t
#define word uint16_t
#define uint8 uint8_t
#define int8 uint8_t
#define uint16 uint16_t
#define int16 int16_t
#define uint32 uint32_t
#define int32 int32_t

#ifdef _posix
    #include "posix_common.h"
#elif defined(PIC32)
    //PIC32 Include
#else
#error "Unsupported platform."
#endif

#define C_FLAG 4
#define H_FLAG 5
#define N_FLAG 6
#define Z_FLAG 7

#define forever for(;;)

#endif
