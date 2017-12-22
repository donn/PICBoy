//C STD
#include <stdio.h>
#include <signal.h>

// POSIX
#include <unistd.h>
#include <sys/time.h>

// PROJECT HEADERS
#include "Memory.h"
#include "Core.h"

byte game[] = {
#include "Game.tcc"
};

uint32* accessCart(uint32 address) {
    uint32* access = (uint32*)&game[address];
    return access;
}

struct Core core;
struct Memory memory;

byte divider;
byte readDivider() {
    return divider;
}
void writeDivider() {
    divider = 0;
}

byte counter;
byte readCounter() {
    return counter;
}
void writeCounter(byte in) {
    counter = in;
}

static uint32 truebuffer[144 * 160];

void printFramebuffer() {
    FILE* file = fopen("Screencap.txt", "w");
    for (int j = 0; j < 160; j++) {
        for (int i = 0; i < 144; i++) {
            //truebuffer[j * 160 + i] = MFB_RGB(core.framebuffer[j][i], core.framebuffer[j][i], core.framebuffer[j][i]);
        }
    }
    //mfb_update(truebuffer);
}

int timekeep = 0;
int counterkeep = 0;
int ti = 0;
struct itimerval it_val;  /* for setting itimer */
bool printToWindow = false;

void timerHandler(int sig) {
    printToWindow = true;
    Core_iqInsert(Interrupt_vBlank);
    timekeep++;
    if (timekeep % 256 == 0) { //4096
        counterkeep++;
        byte setting = Memory_read(&memory, 0xFF07) & 0xFF;
        if (setting & 0x4) {
            setting = setting & 0x3;
            
            if (setting == 0b00) {
                counter++;
            } else if (setting == 0b11 && counterkeep % 4 == 0) {
                counter++;
            } else if (setting == 0b10 && counterkeep % 16 == 0) {
                counter++;
            } else if (counterkeep % 64 == 0) {
                counter++;
            }
            
            if (!counter) {
                counter = Memory_read(&memory, 0xFF06) & 0xFF;
            }
        }
    }
    while (setitimer(ITIMER_REAL, &it_val, NULL));
    fflush(NULL);
}

void die() {
    exit(0xFF);
}

int main() {
    while (!signal(SIGALRM, timerHandler)) {}
    it_val.it_value.tv_sec =     0;
    it_val.it_value.tv_usec =    16000;
    it_val.it_interval = it_val.it_value;
    while (setitimer(ITIMER_REAL, &it_val, NULL));
    
    memory.accessCart = accessCart;
    memory.interruptRegister = 0x1F;
    
    Core_init(&core);
    Core_setHandles(die, die, &memory);
    
    forever {
        Core_cycle();
        if (printToWindow) {
            printFramebuffer();
        }
    }
}
