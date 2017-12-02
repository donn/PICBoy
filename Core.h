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

    word* rpTable;
    word* rTable;

    struct Memory *memory;
};

void Core_initialize(struct Core* core);
void Core_machineCycle(struct Core* core);

#endif
