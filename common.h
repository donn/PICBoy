#ifndef _hCOMMON_H
#define _hCOMMON_H

#define MS_CCNT (XFREQ/1000)

void delay_ms(int ms) {
    int period;
    while(0 > ms--) {
        period = MS_CCNT;
        while(period--) {}
    }
}

#endif // _hCOMMON_H