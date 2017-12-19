#include "plib.h"

#include "Types.h"
#define NAMESPACE pboy
#include "display.h"

static inline
int setup() {
    OpenSPI1(MASTER_ENABLE_ON, SPI_ENABLE);
    
    pboy_init();
    // pboy_setRotation(sdUp);
    pboy_fillScreen(150);
    return 1;
}

static inline
int cleanup() {
    CloseSPI1();
}


int main() {

    if (!setup())
        return -1;

    byte c = 0;
    // Sim loop
    forever {
        for (int i = 0; i < 1000000; ++i) {}
        pboy_fillScreen(c);
        c = (c + 10) % 255;
    }

    cleanup();
    return 0;
}