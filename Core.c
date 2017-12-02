//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
//http://www.z80.info/decoding.htm
//http://fms.komkon.org/GameBoy/Tech/Software.html
#include "Core.h"
#include "Memory.h"

static byte* rTable(struct Core* core, byte y) {
    switch (y) {
        case 7:
            return &core->registers.octets.A;
        case 6:
            return Memory_point(core->memory, core->registers.sedectets.HL);
        case 5:
            return &core->registers.octets.L;
        case 4:
            return &core->registers.octets.H;
        case 3:
            return &core->registers.octets.E;
        case 2:
            return &core->registers.octets.D;
        case 1:
            return &core->registers.octets.C;
        default:
            return &core->registers.octets.B;
    }
}

#define rp2Table(core, p) &core->registers.sedectetArray[(y + 1) % 4]

static void jr(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q);

static void jrc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
//Conditional return
    switch (y) {
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jr(core, instruction, x, y, z, p, q);
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jr(core, instruction, x, y, z, p, q);
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jr(core, instruction, x, y, z, p, q);
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jr(core, instruction, x, y, z, p, q);
            }
    }
}
static void jr(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.sedectets.PC += SIGN_EXTEND((instruction >> 8) & 0xFF);
}
static void stop(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    Memory_write(core->memory, (instruction >> 8) & 0xFF, core->registers.sedectets.SP, false);
}
static void nop(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //No operation
}

static void ldi16(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Load following immediate into RegisterPair[p]
    core->rpTable[p] = (instruction >> 8) & 0xFFFF;
}
static void add16(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void ild(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Indirect loading
    if (q) {
        switch (p) {
            case 0:
                Memory_write(core->memory, core->registers.sedectets.BC, core->registers.octets.A, true);
                break;
            case 1:
                Memory_write(core->memory, core->registers.sedectets.DE, core->registers.octets.A, true);
                break;
            case 2:
                core->registers.sedectets.PC += 2;
                Memory_write(core->memory, (instruction >> 8) & 0xFF, core->registers.sedectets.HL, false);
                break;
            default:
                core->registers.sedectets.PC += 2;
                Memory_write(core->memory, (instruction >> 8) & 0xFF, core->registers.octets.A, true);
        }

    } else {
        switch (p) {
            case 0:
                core->registers.octets.A = Memory_read(core->memory, core->registers.sedectets.BC) & 0xFF;
                break;
            case 1:
                core->registers.octets.A = Memory_read(core->memory, core->registers.sedectets.DE) & 0xFF;
                break;
            case 2:
                core->registers.sedectets.PC += 2;
                core->registers.sedectets.HL = Memory_read(core->memory, (instruction >> 8) & 0xFF) & 0xFFFF;
                break;
            default:
                core->registers.sedectets.PC += 2;
                core->registers.octets.A = Memory_read(core->memory, (instruction >> 8) & 0xFF) & 0xFF;
        }
    }
}
static void ildm(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //p = 0: save at [HL], p = 1: save at A
    //q = 2: increment HL, q = 3: decrement HL
    byte* target = rTable(core, 6 + p);
    if (q == 3) {
        core->registers.sedectets.HL--;
    } else {
        core->registers.sedectets.HL++;
    }
}

static void inc16(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Increment register pair
    core->rpTable[p]++;
}
static void dec16(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Decrement register pair
    core->rpTable[p]--;
}

static void inc8(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Increment register
    rTable(core, y)[0]++;
}

static void dec8(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Decrement register
    rTable(core, y)[0]--;
}

static void ldi8(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Load immediate
    core->registers.sedectets.PC += 1;
    rTable(core, y)[0] = (instruction >> 8) & 0xFF;
}

//ZNHC0000
static void rlca(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Rotate left, old bit 7 set to carry and new bit 0
    bit last = core->registers.octets.A >> 7;
    core->registers.octets.A <<= 1;
    core->registers.octets.A |= last;

    if (last) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void rrca(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Rotate right, old bit 0 set to carry and new bit 7
    bit first = core->registers.octets.A & 1;
    core->registers.octets.A >>= 1;
    core->registers.octets.A |= (first << 7);

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void rla(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
//Rotate left, old bit 7 set to carry and new bit 0
    bit last = core->registers.octets.A >> 7;
    core->registers.octets.A <<= 1;
    core->registers.octets.A |= TEST_BIT(core->registers.octets.F, 4);

    if (last) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void rra(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Rotate right, old bit 0 set to carry and new bit 7
    bit first = core->registers.octets.A & 1;
    core->registers.octets.A >>= 1;
    core->registers.octets.A |= (TEST_BIT(core->registers.octets.F, 4) << 7);

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void daa(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Binary coded decimal adjust accumulator
    byte units = core->registers.octets.A % 10;
    byte tens  = core->registers.octets.A / 10;
    bool carry = false;
    if (tens > 10) {
        tens /= 10;
        carry = true;
    }

    core->registers.octets.A = (tens << 4) | units;

    if (carry) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void cpl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.octets.A = ~core->registers.octets.A;

    SET_BIT(core->registers.octets.F, N_FLAG);
    SET_BIT(core->registers.octets.F, H_FLAG);
}
static void scf(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //I can now see why people appreciate RISC
    SET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void ccf(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    if (TEST_BIT(core->registers.octets.F, 4)) {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, C_FLAG);
    }
    
    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}


static void ld(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Load (i.e. move)
    rTable(core, y)[0] = rTable(core, z)[0];
}

static void halt(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Catch fire or something idfk
}


static void add(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    byte original = core->registers.octets.A;
    core->registers.octets.A += rTable(core, z)[0];

    if ((core->registers.octets.A & 0xF) < (original & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.octets.A < original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void adc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    byte original = core->registers.octets.A;
    core->registers.octets.A += (rTable(core, z)[0] + TEST_BIT(core->registers.octets.F, C_FLAG));

    if ((core->registers.octets.A & 0xF) < (original & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.octets.A < original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void sub(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    byte original = core->registers.octets.A;
    core->registers.octets.A -= rTable(core, z)[0];

    if ((core->registers.octets.A & 0xF) > (original & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.octets.A > original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    SET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void sbc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    byte original = core->registers.octets.A;
    core->registers.octets.A -= (rTable(core, z)[0] + TEST_BIT(core->registers.octets.F, C_FLAG));

    if ((core->registers.octets.A & 0xF) > (original & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.octets.A > original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    SET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void and(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.octets.A &= rTable(core, z)[0];

    SET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void xor(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.octets.A ^= rTable(core, z)[0];

    RESET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void or(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.octets.A |= rTable(core, z)[0];

    RESET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void cp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    byte new = core->registers.octets.A - rTable(core, z)[0];

    if ((new & 0xF) > (core->registers.octets.A & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (new > core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    SET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!new) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}

static void ret(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q);

static void retcc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Conditional return
    switch (y) {
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                ret(core, instruction, x, y, z, p, q);
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                ret(core, instruction, x, y, z, p, q);
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                ret(core, instruction, x, y, z, p, q);
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                ret(core, instruction, x, y, z, p, q);
            }
    }
}

static void pop(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Pop from stack, place in register pair
    *rp2Table(core, p)= Memory_read(core->memory, core->registers.sedectets.SP) & 0xFF;
    core->registers.sedectets.SP += 2;
}
static void ret(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Pop from stack, jump
    core->registers.sedectets.PC = Memory_read(core->memory, core->registers.sedectets.SP) & 0xFF;
    core->registers.sedectets.SP += 2;
}
static void ldhna(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Write A to memory[0xFF00 + n]
    Memory_write(core->memory, 0xFF00 + ((instruction >> 8) & 0xFF), core->registers.octets.A, true);
}
static void ldhan(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Write memory[0xFF00 + n] to A
    core->registers.octets.A = Memory_read(core->memory, 0xFF00 + ((instruction >> 8) & 0xFF)) & 0xFF;
}
static void ldhlsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {

}
static void addsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void jphl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.sedectets.PC = core->registers.sedectets.HL;
}
static void reti(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldsphl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Puts HL into SP (Don't ask. Please.)
    core->registers.sedectets.SP = core->registers.sedectets.HL;
}
static void ldhl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    //Puts SP+n in to HL (I'm starting to think maybe this wasn't such a good idea)
    word original = core->registers.sedectets.SP;
    core->registers.sedectets.HL = core->registers.sedectets.SP + SIGN_EXTEND((instruction >> 8) & 0xFF);

    if ((core->registers.sedectets.HL & 0xFF) < (original & 0xFF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.sedectets.HL < original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, Z_FLAG);
}
static void adc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    word original = core->registers.sedectets.SP;
    core->registers.octets.A += (rTable(core, z)[0] + TEST_BIT(core->registers.octets.F, C_FLAG));

    if ((core->registers.octets.A & 0xF) < (original & 0xF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.octets.A < original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    
    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}

static void jpc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldff00ca(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldwa(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldaw(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void jp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void cb(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void out(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void in(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ex(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void di(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ei(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void callc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void push(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
    core->registers.sedectets.SP -= 2;
    Memory_write(core->memory, core->registers.sedectets.SP, *rp2Table(core, p), true);
}

static void call(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void addi(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void adci(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void subi(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void sbci(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void andi(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void xori(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ori(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void cpi(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void rst(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

void (*execute[])(struct Core*, uint32, byte, byte, byte, byte, byte) = {
    nop /*0*/, ldi16 /*1*/, ild /*2*/, inc16 /*3*/, inc8 /*4*/, dec8 /*5*/, ldi8 /*6*/, rlca /*7*/, 
    ldsp /*8*/, add16 /*9*/, ild /*a*/, dec16 /*b*/, inc8 /*c*/, dec8 /*d*/, ldi8 /*e*/, rrca /*f*/, 
    stop /*10*/, ldi16 /*11*/, ild /*12*/, inc16 /*13*/, inc8 /*14*/, dec8 /*15*/, ldi8 /*16*/, rla /*17*/, 
    jr /*18*/, add16 /*19*/, ild /*1a*/, dec16 /*1b*/, inc8 /*1c*/, dec8 /*1d*/, ldi8 /*1e*/, rra /*1f*/, 
    jrc /*20*/, ldi16 /*21*/, ildm /*22*/, inc16 /*23*/, inc8 /*24*/, dec8 /*25*/, ldi8 /*26*/, daa /*27*/, 
    jrc /*28*/, add16 /*29*/, ildm /*2a*/, dec16 /*2b*/, inc8 /*2c*/, dec8 /*2d*/, ldi8 /*2e*/, cpl /*2f*/, 
    jrc /*30*/, ldi16 /*31*/, ildm /*32*/, inc16 /*33*/, inc8 /*34*/, dec8 /*35*/, ldi8 /*36*/, scf /*37*/, 
    jrc /*38*/, add16 /*39*/, ildm /*3a*/, dec16 /*3b*/, inc8 /*3c*/, dec8 /*3d*/, ldi8 /*3e*/, ccf /*3f*/, 
    ld /*40*/, ld /*41*/, ld /*42*/, ld /*43*/, ld /*44*/, ld /*45*/, ld /*46*/, ld /*47*/, 
    ld /*48*/, ld /*49*/, ld /*4a*/, ld /*4b*/, ld /*4c*/, ld /*4d*/, ld /*4e*/, ld /*4f*/, 
    ld /*50*/, ld /*51*/, ld /*52*/, ld /*53*/, ld /*54*/, ld /*55*/, ld /*56*/, ld /*57*/, 
    ld /*58*/, ld /*59*/, ld /*5a*/, ld /*5b*/, ld /*5c*/, ld /*5d*/, ld /*5e*/, ld /*5f*/, 
    ld /*60*/, ld /*61*/, ld /*62*/, ld /*63*/, ld /*64*/, ld /*65*/, ld /*66*/, ld /*67*/, 
    ld /*68*/, ld /*69*/, ld /*6a*/, ld /*6b*/, ld /*6c*/, ld /*6d*/, ld /*6e*/, ld /*6f*/, 
    ld /*70*/, ld /*71*/, ld /*72*/, ld /*73*/, ld /*74*/, ld /*75*/, halt /*76*/, ld /*77*/, 
    ld /*78*/, ld /*79*/, ld /*7a*/, ld /*7b*/, ld /*7c*/, ld /*7d*/, ld /*7e*/, ld /*7f*/, 
    add /*80*/, add /*81*/, add /*82*/, add /*83*/, add /*84*/, add /*85*/, add /*86*/, add /*87*/, 
    adc /*88*/, adc /*89*/, adc /*8a*/, adc /*8b*/, adc /*8c*/, adc /*8d*/, adc /*8e*/, adc /*8f*/, 
    sub /*90*/, sub /*91*/, sub /*92*/, sub /*93*/, sub /*94*/, sub /*95*/, sub /*96*/, sub /*97*/, 
    sbc /*98*/, sbc /*99*/, sbc /*9a*/, sbc /*9b*/, sbc /*9c*/, sbc /*9d*/, sbc /*9e*/, sbc /*9f*/, 
    and /*a0*/, and /*a1*/, and /*a2*/, and /*a3*/, and /*a4*/, and /*a5*/, and /*a6*/, and /*a7*/, 
    xor /*a8*/, xor /*a9*/, xor /*aa*/, xor /*ab*/, xor /*ac*/, xor /*ad*/, xor /*ae*/, xor /*af*/, 
    or /*b0*/, or /*b1*/, or /*b2*/, or /*b3*/, or /*b4*/, or /*b5*/, or /*b6*/, or /*b7*/, 
    cp /*b8*/, cp /*b9*/, cp /*ba*/, cp /*bb*/, cp /*bc*/, cp /*bd*/, cp /*be*/, cp /*bf*/, 
    retcc /*c0*/, pop /*c1*/, jpc /*c2*/, jp /*c3*/, callc /*c4*/, push /*c5*/, addi /*c6*/, rst /*c7*/, 
    retcc /*c8*/, ret /*c9*/, jpc /*ca*/, cb /*cb*/, callc /*cc*/, call /*cd*/, adci /*ce*/, rst /*cf*/, 
    retcc /*d0*/, pop /*d1*/, jpc /*d2*/, nop /*d3*/, callc /*d4*/, push /*d5*/, subi /*d6*/, rst /*d7*/, 
    retcc /*d8*/, reti /*d9*/, jpc /*da*/, nop /*db*/, callc /*dc*/, nop /*dd*/, sbci /*de*/, rst /*df*/, 
    ldhna /*e0*/, pop /*e1*/, ldff00ca /*e2*/, nop /*e3*/, nop /*e4*/, push /*e5*/, andi /*e6*/, rst /*e7*/, 
    ret /*e8*/, jphl /*e9*/, ldwa /*ea*/, nop /*eb*/, nop /*ec*/, nop /*ed*/, xori /*ee*/, rst /*ef*/, 
    ldhan /*f0*/, pop /*f1*/, nop /*f2*/, di /*f3*/, nop /*f4*/, push /*f5*/, ori /*f6*/, rst /*f7*/, 
    ldhl /*f8*/, ldsphl /*f9*/, ldaw /*fa*/, ei /*fb*/, nop /*fc*/, nop /*fd*/, cpi /*fe*/, rst /*ff*/

};

void Core_initialize(struct Core* core) {
    core->registers.sedectets.PC = 0x0000;
    core->rpTable = &core->registers.sedectets.BC;
}

void Core_machineCycle(struct Core* core) {
    uint32 instruction = Memory_read(core->memory, core->registers.sedectets.PC);
    core->registers.sedectets.PC += 1;

    byte opcode = instruction & 0xFF;
    byte x = (opcode >> 6);
    byte y = (opcode >> 3) & 7;
    byte z = (opcode     ) & 7;
    byte p = y >> 1;
    byte q = y & 1;

    execute[opcode](core, instruction, x, y, z, p, q);
}
