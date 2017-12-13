#ifndef _memory_h
#define _memory_h
#pragma once
#include "Types.h"

struct Memory {
    byte vram[8192];
    byte sram[8192];
    uint32* (*accessCart)(uint32);
    byte interruptRegister;

    union {
        byte array[76];
        struct {
            byte p1;
            byte sb;
            byte sc;
            byte div;
            byte tima;
            byte tma;
            byte tac;
            byte if_;
            byte nr10;
            byte nr11;
            byte nr12;
            byte nr13;
            byte nr14;
            byte nr21;
            byte nr22;
            byte nr23;
            byte nr24;
            byte nr30;
            byte nr31;
            byte nr32;
            byte nr33;
            byte nr34;
            byte nr41;
            byte nr43;
            byte nr44;
            byte nr50;
            byte wavePatternRAM[16];
            byte lcdc;
            byte stat;
            byte scy;
            byte scx;
            byte ly;
            byte lyc;
            byte dma;
            byte bgp;
            byte obp0;
            byte obp1;
            byte wy;
            byte wx;
        } byName;
    } ioRegisters;
};

void Memory_write(struct Memory* memory, word address, word data, bool singleByte);
uint32 Memory_read(struct Memory* memory, word address);
byte* Memory_point(struct Memory* memory, word address);

#endif
