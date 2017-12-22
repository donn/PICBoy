#ifndef _core_h
#define _core_h

#include "Common.h"

#define INTERRUPT_QUEUE_SIZE 32

enum InterruptType {
    Interrupt_NULL = 0,
    Interrupt_vBlank,
    Interrupt_lcdc,
    Interrupt_timer,
    Interrupt_serialTransferComplete,
    Interrupt_negedgeP1X
};

struct Core { //Static Singleton
    //Registers
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

        byte octetArray[12];
        word sedectetArray[6];
    } registers;
    
    //Buffers, memories and other delusions and abominations
    struct Memory *memory;
    
    //"GPU"
    byte framebuffer[160][144];

    //Oh look it's where we handle the interrupts
    bool propagateEnableInterrupts, propagateDisableInterrupts;
    bool interrupts;
    
    int8 interruptQueue[INTERRUPT_QUEUE_SIZE];
    int8 iqFront, iqRear, iqSize;

    //Quick, dirty, temporary storage
    uint32 instruction;
    byte opcode;
    
    //Main program-assigned stop/halt behaviors for "portability" or whatever
    void (*stop)();
    void (*halt)();
};

void Core_init(struct Core* incoming);
void Core_setHandles(void (*stop)(), void (*halt)(), struct Memory *memory);
void Core_cycle();
void Core_iqInsert(enum InterruptType item);

#endif
