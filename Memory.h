#ifndef _memory_h
#define _memory_h
#pragma once
#include "Common.h"

struct Memory { //all ranges extended by 3 to allow for 32-bit accesses
    byte vram[8195];
    byte sram[8195];
    byte stack[130];
    byte spriteAttributes[256];
    uint32* (*accessCart)(uint32);
    byte interruptRegister;
    uint32 garbage;

    union {
        byte array[79];
    } ioRegisters;
};

void Memory_write(struct Memory* memory, word address, word data, bool singleByte);
uint32 Memory_read(struct Memory* memory, word address);
byte* Memory_point(struct Memory* memory, word address);

#endif
