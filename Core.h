#ifndef _core_h
#define _core_h

#include "Types.h"

struct Core { //Static Singleton
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

        // !FIXME! `word` should be `byte`?
        // byte octetArray[12];
        word octetArray[12];
        word sedectetArray[6];
    } registers;

    bool propagateEnableInterrupts, propagateDisableInterrupts;
    bool interrupts;

    uint32 instruction;
    byte opcode;

    void (*stop)();
    void (*halt)();

    struct Memory *memory;
};

void Core_init();
void Core_setHandles(void (*stop)(), void (*halt)(), struct Memory *memory);
void Core_cycle();

#endif
