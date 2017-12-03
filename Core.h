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
            word AF;
            word BC;
            word DE;
            word HL;
            word SP;
            word PC;
        } sedectets;

        word octetArray[12];
        word sedectetArray[6];
    } registers;

    bool propagateEnableInterrupts, propagateDisableInterrupts;
    bool interrupts;

    //We need to cull all the stack garbage from function calls
    uint32 instruction;
    byte x, y, z, p, q;

    struct Memory *memory;
};

void Core_initialize(struct Core* core);
void Core_cycle(struct Core* core);

#endif
