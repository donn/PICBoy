#ifndef _memory_h
#define _memory_h
#include "Types.h"

struct Memory {
    byte vram[8192];
    byte sram[8192];
    uint32 (*accessCart)(uint32);
    byte interruptEnableRegister;
};

void Memory_write(struct Memory* memory, word address, word data, bool singleByte);
uint32 Memory_read(struct Memory* memory, word address);
byte* Memory_point(struct Memory* memory, word address);

#endif
