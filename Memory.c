#include "Memory.h"

uint32 Memory_read(struct Memory* memory, word address) {
    if ((0x0000 <= address) && (address < 0x4000)) {
        //ROM BANK #0
        return *memory->accessCart(address);
    } else if ((0x4000 <= address) && (address < 0x8000)) {
        //SWITCHABLE ROM BANK
        //TO-DO: MBC
        return *memory->accessCart(address + 0x4000);
    } else if ((0x8000 <= address) && (address < 0xA000)) {
        //VRAM 
        uint32* access = (uint32*)&memory->vram[address % 8192];
        return *access;
    } else if ((0xA000 <= address) && (address < 0xC000)) {
        //SWITCHABLE RAM BANK
        //TO-DO: MBC
        return 0;
    } else if ((0xC000 <= address) && (address < 0xFE00)) {
        //INTERNAL RAM (WRAPS AROUND)
        uint32* access = (uint32*)&memory->sram[address % 8192];
        return *access;
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        //TO-DO: Sprite attributes
        return 0; 
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
        return 0;
    } else if ((0xFF00 <= address) && (address < 0xFF4C)) {
        //I/O PORTS
        uint32* access = (uint32*)&memory->ioRegisters.array[address & 0xFF];
        return *access;
    } else if ((0xFF4C <= address) && (address < 0xFF80)) {
        //EMPTY
        return 0;
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL RAM (WHAT THE ACTUAL HELL)
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
        return (byte*)memory->accessCart(address + 0x4000);
    } else if ((0x8000 <= address) && (address < 0xA000)) {
        //VRAM 
        return &memory->vram[address % 8192];
    } else if ((0xA000 <= address) && (address < 0xC000)) {
        //SWITCHABLE RAM BANK
        //TO-DO: MBC
        return NULL;
    } else if ((0xC000 <= address) && (address < 0xFE00)) {
        //INTERNAL RAM (WRAPS AROUND)
        return &memory->sram[address % 8192];
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        //TO-DO: Sprite attributes
        return 0; 
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
        return 0;
    } else if ((0xFF00 <= address) && (address < 0xFF4C)) {
        //I/O PORTS
        return &memory->ioRegisters.array[address & 0xFF];
    } else if ((0xFF4C <= address) && (address < 0xFF80)) {
        //EMPTY
        return 0;
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL RAM (WHAT THE ACTUAL HELL)
        return 0;
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
            memory->vram[address % 8192] = data & 0xFF;
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
            memory->sram[address % 8192] = data & 0xFF;
        } else {
            word* access = (word*)&memory->sram[address % 8192];
            *access = data;
        }
    } else if ((0xFE00 <= address) && (address < 0xFEA0)) {
        //SPRITE ATTRIBUTE MEMORY (OAM)
        //TO-DO: Sprite attributes
    } else if ((0xFEA0 <= address) && (address < 0xFF00)) {
        //EMPTY
    } else if ((0xFF00 <= address) && (address < 0xFF4C)) {
        //I/O PORTS
        if (singleByte) {
            memory->ioRegisters.array[address & 0xFF] = data & 0xFF;
        } else {
            word* access = (word*)&memory->ioRegisters.array[address & 0xFF];
            *access = data;
        }
    } else if ((0xFF4C <= address) && (address < 0xFF80)) {
        //EMPTY
    } else if ((0xFF80 <= address) && (address < 0xFFFF)) {
        //INTERNAL RAM (WHAT THE ACTUAL HELL)
    } else {
        //INTERRUPT ENABLE REGISTER
        memory->interruptRegister = data & 0xFF;
    }
}
