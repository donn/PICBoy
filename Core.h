#ifndef _core_h
#define _core_h

#include "Types.h"

struct Core {
    union {
        struct {
            byte A;
            byte F;
            byte B;
            byte C;
            byte D;
            byte E;
            byte H;
            byte L;
            byte unused[4];
        } octets;

        struct {
            short AF;
            short BC;
            short DE;
            short HL;
            short SP;
            short PC;
        } sedectets;
    } registers;

    short* rpTable;
    short* rTable;

    struct Memory *memory;
};

void Core_initialize(struct Core* core);
void Core_machineCycle(struct Core* core);

#endif
