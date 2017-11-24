#include <stdio.h>

#include "Memory.h"

const byte game[] = {
#include "Game.tcc"
};

uint32 accessCart(uint32 address) {
    uint32* access = (uint32*)&game[address];
    return *access;
}

int main() {
    uint32 address = 0;
    struct Memory* memory = malloc(sizeof(struct Memory));
    memory->accessCart = accessCart;
    while (address < sizeof(game) - 3) {
        printf("%08x\n", Memory_read(memory, address++));
    }
}
