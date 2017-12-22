#include "Memory.h"

#include <stdio.h>

uint32 Memory_read(struct Memory* memory, word address) {
    if ((0x0000 <= address) && (address < 0x4000)) {
        //ROM BANK #0
        return *memory->accessCart(address);
    } else if ((0x4000 <= address) && (address < 0x8000)) {
        //SWITCHABLE ROM BANK
        //TO-DO: MBC
        return *memory->accessCart(address);
    } else if ((0x8000 <= address) && (address < 0xA000)) {
        //VRAM 
        uint32* access = (uint32*)&memory->vram[address - 0x8000];
        return *access;
    } else if ((0xA000 <= address) && (address < 0xC000)) {
        //SWITCHABLE RAM BANK
        //TO-DO: MBC
        PICBOY_LOG("Attempted to read from switchable bank\n");
        return 0;
    } else if ((0xC000 <= address) && (address < 0xFE00)) {
        //INTERNAL RAM (WRAPS AROUND)
        uint32* access = (uint32*)&memory->sram[address - 0xC000];
        return *access;
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        uint32* access = (uint32*)&memory->spriteAttributes[address - 0xFE00];
        return *access;
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
        PICBOY_LOG("Attempted to read from unusuable memory\n");
        return 0;
    } else if ((0xFF00 <= address) && (address < 0xFF80)) {
        //I/O PORTS
        uint32* access = (uint32*)&memory->ioRegisters.array[address & 0xFF];
        return *access;
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL STACK
        uint32* access = (uint32*)&memory->sram[address & 0x7F];
        return 0;
    } else {
        //INTERRUPT ENABLE REGISTER
        return memory->interruptRegister;
    }
}

byte* Memory_point(struct Memory* memory, word address) {
    if ((0x0000 <= address) && (address < 0x4000)) {
        //ROM BANK #0
        return (byte*)memory->accessCart(address);
    } else if ((0x4000 <= address) && (address < 0x8000)) {
        //SWITCHABLE ROM BANK
        //TO-DO: MBC
        return (byte*)memory->accessCart(address);
    } else if ((0x8000 <= address) && (address < 0xA000)) {
        //VRAM 
        return &memory->vram[address - 0x8000];
    } else if ((0xA000 <= address) && (address < 0xC000)) {
        //SWITCHABLE RAM BANK
        //TO-DO: MBC
        PICBOY_LOG("Attempted to point to switchable bank\n");
        return (byte*)&memory->garbage;
    } else if ((0xC000 <= address) && (address < 0xFE00)) {
        //INTERNAL RAM (WRAPS AROUND)
        return &memory->sram[address - 0xC000];
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        return &memory->spriteAttributes[address - 0xFE00];
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
        PICBOY_LOG("Attempted to point to unusable memory\n");
        return (byte*)&memory->garbage;
    } else if ((0xFF00 <= address) && (address < 0xFF4C)) {
        //I/O PORTS
        return &memory->ioRegisters.array[address & 0xFF];
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL STACK
        return &memory->sram[address & 0x7F];
    } else {
        //INTERRUPT ENABLE REGISTER
        return &memory->interruptRegister;
    }
}

void Memory_write(struct Memory* memory, word address, word data, bool singleByte) {
    //No point honoring requests addressed before this point
    if ((0x8000 <= address) && (address < 0xA000)) {
        //VRAM
        if (singleByte) {
            memory->vram[address - 0x8000] = data & 0xFF;
        } else {
            word* access = (word*)&memory->vram[address % 8192];
            *access = data;
        }
    } else if ((0xA000 <= address) && (address < 0xC000)) {
        //SWITCHABLE RAM BANK
        //TO-DO: MBC
    } else if ((0xC000 <= address) && (address < 0xFE00)) {
        //INTERNAL RAM (WRAPS AROUND)
        if (singleByte) {
            memory->sram[address - 0xC000] = data & 0xFF;
        } else {
            word* access = (word*)&memory->sram[address - 0xC000];
            *access = data;
        }
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        if (singleByte) {
            memory->spriteAttributes[address - 0xFE00] = data & 0xFF;
        } else {
            word* access = (word*)&memory->spriteAttributes[address - 0xFE00];
            *access = data;
        }
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
        PICBOY_LOG("Attempted to write to unusable memory\n");
    } else if ((0xFF00 <= address) && (address < 0xFF80)) {
        //I/O PORTS
        if (singleByte) {
            memory->ioRegisters.array[address & 0xFF] = data & 0xFF;
        } else {
            word* access = (word*)&memory->ioRegisters.array[address & 0xFF];
            *access = data;
        }
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL STACK
        if (singleByte) {
            memory->sram[address & 0x7F] = data & 0xFF;
        } else {
            word* access = (word*)&memory->sram[address & 0x7F];
            *access = data;
        }
    } else {
        //INTERRUPT ENABLE REGISTER
        memory->interruptRegister = data & 0xFF;
    }
}
