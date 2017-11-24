//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//http://www.z80.info/decoding.htm
//http://fms.komkon.org/GameBoy/Tech/Software.html
#include "Core.h"

static void jrc(struct Core* core, uint32 instruction) {
}
static void jr(struct Core* core, uint32 instruction) {
}
static void stop(struct Core* core, uint32 instruction) {
}
static void ldsp(struct Core* core, uint32 instruction) {
}
static void nop(struct Core* core, uint32 instruction) {
}

static void ldi16(struct Core* core, uint32 instruction) {
}
static void add16(struct Core* core, uint32 instruction) {
}

static void ild(struct Core* core, uint32 instruction) {
}

static void inc16(struct Core* core, uint32 instruction) {
}
static void dec16(struct Core* core, uint32 instruction) {
}

static void inc8(struct Core* core, uint32 instruction) {
}

static void dec8(struct Core* core, uint32 instruction) {
}

static void ldi8(struct Core* core, uint32 instruction) {
}

static void rlca(struct Core* core, uint32 instruction) {
}
static void rrca(struct Core* core, uint32 instruction) {
}
static void rla(struct Core* core, uint32 instruction) {
}
static void rra(struct Core* core, uint32 instruction) {
}
static void daa(struct Core* core, uint32 instruction) {
}
static void cpl(struct Core* core, uint32 instruction) {
}
static void scf(struct Core* core, uint32 instruction) {
}
static void ccf(struct Core* core, uint32 instruction) {
}


static void ld(struct Core* core, uint32 instruction) {
}

static void halt(struct Core* core, uint32 instruction) {
}


static void add(struct Core* core, uint32 instruction) {
}
static void adc(struct Core* core, uint32 instruction) {
}
static void sub(struct Core* core, uint32 instruction) {
}
static void sbc(struct Core* core, uint32 instruction) {
}
static void and(struct Core* core, uint32 instruction) {
}
static void xor(struct Core* core, uint32 instruction) {
}
static void or(struct Core* core, uint32 instruction) {
}
static void cp(struct Core* core, uint32 instruction) {
}


static void retc(struct Core* core, uint32 instruction) {
}

static void pop(struct Core* core, uint32 instruction) {
}
static void ret(struct Core* core, uint32 instruction) {
}
static void jphl(struct Core* core, uint32 instruction) {
}
static void exx(struct Core* core, uint32 instruction) {
}
static void ldsphl(struct Core* core, uint32 instruction) {
}

static void jp(struct Core* core, uint32 instruction) {
}
static void cb(struct Core* core, uint32 instruction) {
}
static void out(struct Core* core, uint32 instruction) {
}
static void in(struct Core* core, uint32 instruction) {
}
static void exsp(struct Core* core, uint32 instruction) {
}
static void ex(struct Core* core, uint32 instruction) {
}
static void di(struct Core* core, uint32 instruction) {
}
static void ei(struct Core* core, uint32 instruction) {
}

static void callc(struct Core* core, uint32 instruction) {
}

static void push(struct Core* core, uint32 instruction) {
}

static void call(struct Core* core, uint32 instruction) {
}
static void unimp(struct Core* core, uint32 instruction) {
}

static void addi(struct Core* core, uint32 instruction) {
}
static void adci(struct Core* core, uint32 instruction) {
}
static void subi(struct Core* core, uint32 instruction) {
}
static void sbci(struct Core* core, uint32 instruction) {
}
static void andi(struct Core* core, uint32 instruction) {
}
static void xori(struct Core* core, uint32 instruction) {
}
static void ori(struct Core* core, uint32 instruction) {
}
static void cpi(struct Core* core, uint32 instruction) {
}

static void rst(struct Core* core, uint32 instruction) {
}

void (*execute[])(struct Core*, uint32) = {
    nop, ldi16, ild, inc16, inc8, dec8, ldi8, rlca, 
    ldsp, add16, ild, dec16, inc8, dec8, ldi8, rrca, 
    stop, ldi16, ild, inc16, inc8, dec8, ldi8, rla, 
    jr, add16, ild, dec16, inc8, dec8, ldi8, rra, 
    jrc, ldi16, ild, inc16, inc8, dec8, ldi8, daa, 
    jrc, add16, ild, dec16, inc8, dec8, ldi8, cpl, 
    jrc, ldi16, ild, inc16, inc8, dec8, ldi8, scf, 
    jrc, add16, ild, dec16, inc8, dec8, ldi8, ccf, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    ld, ld, ld, ld, ld, ld, halt, ld, 
    ld, ld, ld, ld, ld, ld, ld, ld, 
    add, add, add, add, add, add, add, add, 
    adc, adc, adc, adc, adc, adc, adc, adc, 
    sub, sub, sub, sub, sub, sub, sub, sub, 
    sbc, sbc, sbc, sbc, sbc, sbc, sbc, sbc, 
    and, and, and, and, and, and, and, and, 
    xor, xor, xor, xor, xor, xor, xor, xor, 
    or, or, or, or, or, or, or, or, 
    cp, cp, cp, cp, cp, cp, cp, cp, 
    ret, pop, jpc, jp, callc, push, addi, rst, 
    ret, ret, jpc, cb, callc, call, adci, rst, 
    ret, pop, jpc, out, callc, push, subi, rst, 
    ret, exx, jpc, in, callc, unimp, sbci, rst, 
    ret, pop, jpc, exsp, callc, push, andi, rst, 
    ret, jphl, jpc, ex, callc, unimp, xori, rst, 
    ret, pop, jpc, di, callc, push, ori, rst, 
    ret, ldsphl, jpc, ei, callc, unimp, cpi, rst
};

void Core_initialize(Core* core) {
    core->registers.sedectets.PC = 0x0000;
}

void Core_machineCycle(Core* core) {
    word pc = core->registers.sedectets.PC;
    uint32 instruction = Memory_read(core->memory, core->registers.sedectets.PC);
    execute[pc](core, instruction);
}
