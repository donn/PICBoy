#include <stdio.h>

#include "Memory.h"
#include "Core.h"

const byte game[] = {
#include "Game.tcc"
};

uint32* accessCart(uint32 address) {
    uint32* access = (uint32*)&game[address];
    return access;
}

void die() {
    exit(69);
}

int main() {
    printf("%lu\n", sizeof(game));
    struct Memory* memory = malloc(sizeof(struct Memory));
    memory->accessCart = accessCart;
    Core_init();
    Core_setHandles(die, die, memory);

    forever {
        Core_cycle();
    }
}
