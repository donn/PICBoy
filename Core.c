//A lot of the nomenclature in this file are based off the "General Z80 CPU Info" tables/terms/whatevers
#include "Core.h"
#include "Memory.h"

static struct Core* core; 

#define FIELD_X ((core->opcode >> 6))
#define FIELD_Y ((core->opcode >> 3) &  7)
#define FIELD_Z ((core->opcode     ) &  7)
#define FIELD_P ((core->opcode >> 3) >> 1)
#define FIELD_Q ((core->opcode >> 3) &  1)

static byte* rTable(byte y) {
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
#define rpTable(p)  &core->registers.sedectetArray[p + 1]
#define rp2Table(p) &core->registers.sedectetArray[(p + 1) % 4]

//CB-prefixed Instructions

static void testbit() {
    if (TEST_BIT(*rTable(FIELD_Z), FIELD_Y)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    SET_BIT(core->registers.octets.F, H_FLAG);
}

static void setbit() {
    SET_BIT(*rTable(FIELD_Z), FIELD_Y);
}

static void resetbit() {
    RESET_BIT(*rTable(FIELD_Z), FIELD_Y);
}

static void rlcn() {
    //Rotate left, old bit 7 set to carry and new bit 0
    bit last = *rTable(FIELD_Z) >> 7;
    *rTable(FIELD_Z) <<= 1;
    *rTable(FIELD_Z) |= last;

    if (last) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}

static void rrcn() {
    //Rotate right, old bit 0 set to carry and new bit 7
    bit first = *rTable(FIELD_Z) & 1;
    *rTable(FIELD_Z) >>= 1;
    *rTable(FIELD_Z) |= (first << 7);

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}

static void rln() {
    //Rotate left through carry
    bit last = *rTable(FIELD_Z) >> 7;
    *rTable(FIELD_Z) <<= 1;
    *rTable(FIELD_Z) |= TEST_BIT(core->registers.octets.F, C_FLAG);

    if (last) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}

static void rrn() {
    //Rotate right through carry
    bit first = *rTable(FIELD_Z) & 1;
    *rTable(FIELD_Z) >>= 1;
    *rTable(FIELD_Z) |= (TEST_BIT(core->registers.octets.F, C_FLAG) << 7);

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}

static void slan() {
    //Shift left into carry
    bit last = *rTable(FIELD_Z) >> 7;
    *rTable(FIELD_Z) <<= 1;

    if (last) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}

static void sran() {
    //Arithmetic shift right into carry
    bit first = *rTable(FIELD_Z) & 1;
    *rTable(FIELD_Z) = (displacement)*rTable(FIELD_Z) >> 1;

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
}

static void swap() {
    SWAP_NIBBLES(*rTable(FIELD_Z));
    
    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

}

static void srln() {
 //Arithmetic shift right into carry
    bit first = *rTable(FIELD_Z) & 1;
    *rTable(FIELD_Z) >>= 1;

    if (first) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    if (!*rTable(FIELD_Z)) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }

    RESET_BIT(core->registers.octets.F, N_FLAG);
}

void (*cbExecute[])() = {
    rlcn /*00*/, rlcn /*01*/, rlcn /*02*/, rlcn /*03*/, rlcn /*04*/, rlcn /*05*/, rlcn /*06*/, rlcn /*07*/,
    rrcn /*08*/, rrcn /*09*/, rrcn /*0a*/, rrcn /*0b*/, rrcn /*0c*/, rrcn /*0d*/, rrcn /*0e*/, rrcn /*0f*/,
    rln /*10*/, rln /*11*/, rln /*12*/, rln /*13*/, rln /*14*/, rln /*15*/, rln /*16*/, rln /*17*/,
    rrn /*18*/, rrn /*19*/, rrn /*1a*/, rrn /*1b*/, rrn /*1c*/, rrn /*1d*/, rrn /*1e*/, rrn /*1f*/,
    slan /*20*/, slan /*21*/, slan /*22*/, slan /*23*/, slan /*24*/, slan /*25*/, slan /*26*/, slan /*27*/,
    sran /*28*/, sran /*29*/, sran /*2a*/, sran /*2b*/, sran /*2c*/, sran /*2d*/, sran /*2e*/, sran /*2f*/,
    swap /*30*/, swap /*31*/, swap /*32*/, swap /*33*/, swap /*34*/, swap /*35*/, swap /*36*/, swap /*37*/,
    srln /*38*/, srln /*39*/, srln /*3a*/, srln /*3b*/, srln /*3c*/, srln /*3d*/, srln /*3e*/, srln /*3f*/,
    testbit /*40*/, testbit /*41*/, testbit /*42*/, testbit /*43*/, testbit /*44*/, testbit /*45*/, testbit /*46*/, testbit /*47*/,
    testbit /*48*/, testbit /*49*/, testbit /*4a*/, testbit /*4b*/, testbit /*4c*/, testbit /*4d*/, testbit /*4e*/, testbit /*4f*/,
    testbit /*50*/, testbit /*51*/, testbit /*52*/, testbit /*53*/, testbit /*54*/, testbit /*55*/, testbit /*56*/, testbit /*57*/,
    testbit /*58*/, testbit /*59*/, testbit /*5a*/, testbit /*5b*/, testbit /*5c*/, testbit /*5d*/, testbit /*5e*/, testbit /*5f*/,
    testbit /*60*/, testbit /*61*/, testbit /*62*/, testbit /*63*/, testbit /*64*/, testbit /*65*/, testbit /*66*/, testbit /*67*/,
    testbit /*68*/, testbit /*69*/, testbit /*6a*/, testbit /*6b*/, testbit /*6c*/, testbit /*6d*/, testbit /*6e*/, testbit /*6f*/,
    testbit /*70*/, testbit /*71*/, testbit /*72*/, testbit /*73*/, testbit /*74*/, testbit /*75*/, testbit /*76*/, testbit /*77*/,
    testbit /*78*/, testbit /*79*/, testbit /*7a*/, testbit /*7b*/, testbit /*7c*/, testbit /*7d*/, testbit /*7e*/, testbit /*7f*/,
    resetbit /*80*/, resetbit /*81*/, resetbit /*82*/, resetbit /*83*/, resetbit /*84*/, resetbit /*85*/, resetbit /*86*/, resetbit /*87*/,
    resetbit /*88*/, resetbit /*89*/, resetbit /*8a*/, resetbit /*8b*/, resetbit /*8c*/, resetbit /*8d*/, resetbit /*8e*/, resetbit /*8f*/,
    resetbit /*90*/, resetbit /*91*/, resetbit /*92*/, resetbit /*93*/, resetbit /*94*/, resetbit /*95*/, resetbit /*96*/, resetbit /*97*/,
    resetbit /*98*/, resetbit /*99*/, resetbit /*9a*/, resetbit /*9b*/, resetbit /*9c*/, resetbit /*9d*/, resetbit /*9e*/, resetbit /*9f*/,
    resetbit /*a0*/, resetbit /*a1*/, resetbit /*a2*/, resetbit /*a3*/, resetbit /*a4*/, resetbit /*a5*/, resetbit /*a6*/, resetbit /*a7*/,
    resetbit /*a8*/, resetbit /*a9*/, resetbit /*aa*/, resetbit /*ab*/, resetbit /*ac*/, resetbit /*ad*/, resetbit /*ae*/, resetbit /*af*/,
    resetbit /*b0*/, resetbit /*b1*/, resetbit /*b2*/, resetbit /*b3*/, resetbit /*b4*/, resetbit /*b5*/, resetbit /*b6*/, resetbit /*b7*/,
    resetbit /*b8*/, resetbit /*b9*/, resetbit /*ba*/, resetbit /*bb*/, resetbit /*bc*/, resetbit /*bd*/, resetbit /*be*/, resetbit /*bf*/,
    setbit /*c0*/, setbit /*c1*/, setbit /*c2*/, setbit /*c3*/, setbit /*c4*/, setbit /*c5*/, setbit /*c6*/, setbit /*c7*/,
    setbit /*c8*/, setbit /*c9*/, setbit /*ca*/, setbit /*cb*/, setbit /*cc*/, setbit /*cd*/, setbit /*ce*/, setbit /*cf*/,
    setbit /*d0*/, setbit /*d1*/, setbit /*d2*/, setbit /*d3*/, setbit /*d4*/, setbit /*d5*/, setbit /*d6*/, setbit /*d7*/,
    setbit /*d8*/, setbit /*d9*/, setbit /*da*/, setbit /*db*/, setbit /*dc*/, setbit /*dd*/, setbit /*de*/, setbit /*df*/,
    setbit /*e0*/, setbit /*e1*/, setbit /*e2*/, setbit /*e3*/, setbit /*e4*/, setbit /*e5*/, setbit /*e6*/, setbit /*e7*/,
    setbit /*e8*/, setbit /*e9*/, setbit /*ea*/, setbit /*eb*/, setbit /*ec*/, setbit /*ed*/, setbit /*ee*/, setbit /*ef*/,
    setbit /*f0*/, setbit /*f1*/, setbit /*f2*/, setbit /*f3*/, setbit /*f4*/, setbit /*f5*/, setbit /*f6*/, setbit /*f7*/,
    setbit /*f8*/, setbit /*f9*/, setbit /*fa*/, setbit /*fb*/, setbit /*fc*/, setbit /*fd*/, setbit /*fe*/, setbit /*ff*/,
};

static void cb() {
    core->registers.sedectets.PC += 1;
    core->opcode = (core->instruction >> 8) & 0xFF;
    cbExecute[core->opcode]();
}

//Regular Instructions

static void jr() {
    //Jump relatively
    core->registers.sedectets.PC += 1;
    core->registers.sedectets.PC += SIGN_EXTEND((core->instruction >> 8) & 0xFF);
}
static void jrc() {
    //Conditional return
    switch (FIELD_Y) { 
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jr();
                return;
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jr();
                return;
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jr();
                return;
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jr();
                return;
            }
    }
    core->registers.sedectets.PC += 1; //The follower n won't just go on its own
    
}

static void stop() {
    core->stop();
}
static void ldnnsp() {
    //Load sp into memory[nn]
    core->registers.sedectets.PC += 2;
    Memory_write(core->memory, (core->instruction >> 8) & 0xFFFF, core->registers.sedectets.SP, false);
}
static void nop() {
    //No operation
}

static void ldi16() {
    //Load following immediate into register pair
    core->registers.sedectets.PC += 2;
    *rpTable(FIELD_P) = (core->instruction >> 8) & 0xFFFF;

}
static void add16() {
    //Add register pair to HL
    word original = core->registers.sedectets.HL;
    core->registers.sedectets.HL += *rpTable(FIELD_P);

    if ((core->registers.sedectets.HL & 0xFFF) < (original & 0xFFF)) {
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
}

static void ild() {
    //Indirect loading
    //(It's complicated)
    if (FIELD_Q) {
        switch (FIELD_P) {
            case 0:
                Memory_write(core->memory, core->registers.sedectets.BC, core->registers.octets.A, true);
                break;
            case 1:
                Memory_write(core->memory, core->registers.sedectets.DE, core->registers.octets.A, true);
                break;
            case 2:
                core->registers.sedectets.PC += 2;
                Memory_write(core->memory, (core->instruction >> 8) & 0xFFFF, core->registers.sedectets.HL, false);
                break;
            default:
                core->registers.sedectets.PC += 2;
                Memory_write(core->memory, (core->instruction >> 8) & 0xFFFF, core->registers.octets.A, true);
        }

    } else {
        switch (FIELD_P) {
            case 0:
                core->registers.octets.A = Memory_read(core->memory, core->registers.sedectets.BC) & 0xFF;
                break;
            case 1:
                core->registers.octets.A = Memory_read(core->memory, core->registers.sedectets.DE) & 0xFF;
                break;
            case 2:
                core->registers.sedectets.PC += 2;
                core->registers.sedectets.HL = Memory_read(core->memory, (core->instruction >> 8) & 0xFFFF) & 0xFFFF;
                break;
            default:
                core->registers.sedectets.PC += 2;
                core->registers.octets.A = Memory_read(core->memory, (core->instruction >> 8) & 0xFFFF) & 0xFF;
        }
    }
}
static void ildm() {
    //p = 0: save at [HL], p = 1: save at A
    //q = 2: increment HL, q = 3: decrement HL
    byte* target = rTable(6 + FIELD_P);
    int hlchange = (FIELD_Q == 3)? -1: 1;
    if (FIELD_P) {
        core->registers.octets.A = Memory_read(core->memory, core->registers.sedectets.HL) & 0xFF;
    } else {
        Memory_write(core->memory, core->registers.sedectets.HL, core->registers.octets.A, true);
    }
    core->registers.sedectets.HL += hlchange;
}
static void inc16() {
    //Increment register pair
    (*rpTable(FIELD_P))++;
}
static void dec16() {
    //Decrement register pair
    (*rpTable(FIELD_P))--;
}

static void inc8() {
    //Increment register
    (*rTable(FIELD_Y))++;
}

static void dec8() {
    //Decrement register
    (*rTable(FIELD_Y))--;
}

static void ldi8() {
    //Load immediate
    core->registers.sedectets.PC += 1;
    (*rTable(FIELD_Y)) = (core->instruction >> 8) & 0xFF;
}

//ZNHC0000
static void rlca() {
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
static void rrca() {
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
static void rla() {
    //Rotate left through carry
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
static void rra() {
    //Rotate right through carry
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
static void daa() {
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
static void cpl() {
    
    core->registers.octets.A = ~core->registers.octets.A;
    //Complement accumulator
    SET_BIT(core->registers.octets.F, N_FLAG);
    SET_BIT(core->registers.octets.F, H_FLAG);
}
static void scf() {
    //Set carry flag (I can now see why people appreciate RISC)
    SET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}
static void ccf() {
    //Complement carry flag
    if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, C_FLAG);
    }
    
    RESET_BIT(core->registers.octets.F, N_FLAG);
    RESET_BIT(core->registers.octets.F, H_FLAG);
}


static void ld() {
    //Load (i.e. move)
    (*rTable(FIELD_Y)) = (*rTable(FIELD_Z));
}

static void halt() {
    core->halt();
}


static void add() {
    //Add register to accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    byte original = core->registers.octets.A;
    core->registers.octets.A += target;

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
static void adc() {
    //Add register plus carry to accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    byte original = core->registers.octets.A;
    core->registers.octets.A += target + TEST_BIT(core->registers.octets.F, C_FLAG);

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
static void sub() {
    //Subtract register from accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    byte original = core->registers.octets.A;
    core->registers.octets.A -= target;

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
static void sbc() {
    //Subtract register + carry from accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    byte original = core->registers.octets.A;
    core->registers.octets.A -= target + TEST_BIT(core->registers.octets.F, C_FLAG);

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
static void and() {
    //Logically and register with accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    core->registers.octets.A &= target;

    SET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void xor() {
    //Logically xor register with accumulator
    core->registers.octets.A ^= (*rTable(FIELD_Z));

    RESET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void or() {
    //Logically or register with accumulator
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    core->registers.octets.A |= target;

    RESET_BIT(core->registers.octets.F, H_FLAG);
    RESET_BIT(core->registers.octets.F, C_FLAG);

    RESET_BIT(core->registers.octets.F, N_FLAG);

    if (!core->registers.octets.A) {
        SET_BIT(core->registers.octets.F, Z_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, Z_FLAG);
    }
}
static void cp() {
    //Compare register with accumulator (basically sub but the result is thrown away)
    byte target = (*rTable(FIELD_Z));
    if (FIELD_X == 3) {
        target = (core->instruction >> 8) & 0xFF;
    }
    byte new = core->registers.octets.A - target;

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


static void ret() {
    //Pop from stack, jump
    core->registers.sedectets.PC = Memory_read(core->memory, core->registers.sedectets.SP) & 0xFFFF;
    core->registers.sedectets.SP += 2;
}

static void retcc() {
    //Conditional return
    switch (FIELD_Y) {
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                ret();
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                ret();
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                ret();
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                ret();
            }
    }
}
static void reti() {
    //Return and enable interrupts
    ret();
}

static void pop() {
    //Pop from stack, place in register pair
    *rp2Table(FIELD_P) = Memory_read(core->memory, core->registers.sedectets.SP) & 0xFFFF;
    core->registers.sedectets.SP += 2;
}
static void ldhna() {
    //Write A to memory[0xFF00 + n]
    core->registers.sedectets.PC += 1;
    Memory_write(core->memory, 0xFF00 + SIGN_EXTEND((core->instruction >> 8) & 0xFF), core->registers.octets.A, true);
}
static void ldhan() {
    //Write memory[0xFF00 + n] to A
    core->registers.sedectets.PC += 1;
    core->registers.octets.A = Memory_read(core->memory, 0xFF00 + SIGN_EXTEND((core->instruction >> 8) & 0xFF)) & 0xFF;
}
static void addspn() {
    //Add register pair to HL
    word original = core->registers.sedectets.SP;
    core->registers.sedectets.PC += 1;
    core->registers.sedectets.SP += SIGN_EXTEND((core->instruction >> 8) & 0xFF);

    if ((core->registers.sedectets.SP & 0xFFF) < (original & 0xFFF)) {
        SET_BIT(core->registers.octets.F, H_FLAG);
    } else {
        SET_BIT(core->registers.octets.F, H_FLAG);
    }

    if (core->registers.sedectets.SP < original) {
        SET_BIT(core->registers.octets.F, C_FLAG);
    } else {
        RESET_BIT(core->registers.octets.F, C_FLAG);
    }

    RESET_BIT(core->registers.octets.F, Z_FLAG);
    RESET_BIT(core->registers.octets.F, N_FLAG);
}
static void jphl() {
    //Jump to HL
    core->registers.sedectets.PC = core->registers.sedectets.HL;
}
static void ldsphl() {
    //Puts HL into SP (Don't ask. Please.)
    core->registers.sedectets.SP = core->registers.sedectets.HL;
}
static void ldhlspn() {
    //Puts SP+n in to HL (I'm starting to think maybe this wasn't such a good idea)
    word original = core->registers.sedectets.SP;
    core->registers.sedectets.PC += 1;
    core->registers.sedectets.HL = core->registers.sedectets.SP + SIGN_EXTEND((core->instruction >> 8) & 0xFF);

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


static void jp() {
    //Jump to address nn
    core->registers.sedectets.PC += 2; //Does it really matter...
    core->registers.sedectets.PC = (core->instruction >> 8) & 0xFFFF;
}

static void jpc() {
    //Conditional jump
    switch (FIELD_Y) {
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jp();
                return;
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                jp();
                return;
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jp();
                return;
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                jp();
                return;
            }
    }
    core->registers.sedectets.PC += 2; //The follower nn won't just go on its own
}
static void ldwa() {
    //Write accumulator to memory[nn]
    core->registers.sedectets.PC += 2;
    Memory_write(core->memory, (core->instruction >> 8) & 0xFFFF, core->registers.octets.A, true);
}
static void ldaw() {
    //Write memory[nn] to accumulator
    core->registers.sedectets.PC += 2;
    core->registers.octets.A = Memory_read(core->memory, (core->instruction >> 8) & 0xFFFF) & 0xFF;
}
static void ldca() {
    //Write accumulator to memory[0xFF00 + C]
    Memory_write(core->memory, 0xFF00 + SIGN_EXTEND(core->registers.octets.C), core->registers.octets.A, true);
}
static void ldac() {
    //Write memory[0xFF00 + C] to accumulator
    core->registers.octets.A = Memory_read(core->memory, 0xFF00 + SIGN_EXTEND(core->registers.octets.C));
}

static void push() {
    //Push on stack
    core->registers.sedectets.SP -= 2;
    Memory_write(core->memory, core->registers.sedectets.SP, *rp2Table(FIELD_P), true);
}

static void call() {
    //Push next instruction's address to stack, jump to nn
    core->registers.sedectets.SP -= 2;
    core->registers.sedectets.PC += 2;
    Memory_write(core->memory, core->registers.sedectets.SP, core->registers.sedectets.PC, false);
    core->registers.sedectets.PC = (core->instruction >> 8) & 0xFFFF;
}
static void callc() {
    //Conditional call
    switch (FIELD_Y) {
        case 3:
            if (TEST_BIT(core->registers.octets.F, C_FLAG)) {
                call();
                return;
            }
        case 2:
            if (!TEST_BIT(core->registers.octets.F, C_FLAG)) {
                call();
                return;
            }
        case 1:
            if (TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                call();
                return;
            }
        default: 
            if (!TEST_BIT(core->registers.octets.F, Z_FLAG)) {
                call();
                return;
            }
    }
    core->registers.sedectets.PC += 2; //The follower nn won't just go on its own
  
}

static void di() {
    core->propagateDisableInterrupts = true;
}
static void ei() {
    core->propagateEnableInterrupts = true;
}
static void rst() {
    //"Restart": Push address on stack then jump somewhere
    core->registers.sedectets.SP -= 2;
    Memory_write(core->memory, core->registers.sedectets.SP, core->registers.sedectets.PC, false);
    core->registers.sedectets.PC = FIELD_Y << 3;
}

void (*execute[])() = {
    nop /*0*/, ldi16 /*1*/, ild /*2*/, inc16 /*3*/, inc8 /*4*/, dec8 /*5*/, ldi8 /*6*/, rlca /*7*/, 
    ldnnsp /*8*/, add16 /*9*/, ild /*a*/, dec16 /*b*/, inc8 /*c*/, dec8 /*d*/, ldi8 /*e*/, rrca /*f*/, 
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
    retcc /*c0*/, pop /*c1*/, jpc /*c2*/, jp /*c3*/, callc /*c4*/, push /*c5*/, add /*c6*/, rst /*c7*/, 
    retcc /*c8*/, ret /*c9*/, jpc /*ca*/, cb /*cb*/, callc /*cc*/, call /*cd*/, adc /*ce*/, rst /*cf*/, 
    retcc /*d0*/, pop /*d1*/, jpc /*d2*/, nop /*d3*/, callc /*d4*/, push /*d5*/, sub /*d6*/, rst /*d7*/, 
    retcc /*d8*/, reti /*d9*/, jpc /*da*/, nop /*db*/, callc /*dc*/, nop /*dd*/, sbc /*de*/, rst /*df*/, 
    ldhna /*e0*/, pop /*e1*/, ldca /*e2*/, nop /*e3*/, nop /*e4*/, push /*e5*/, and /*e6*/, rst /*e7*/, 
    ret /*e8*/, jphl /*e9*/, ldwa /*ea*/, nop /*eb*/, nop /*ec*/, nop /*ed*/, xor /*ee*/, rst /*ef*/, 
    ldhan /*f0*/, pop /*f1*/, ldac /*f2*/, di /*f3*/, nop /*f4*/, push /*f5*/, or /*f6*/, rst /*f7*/, 
    ldhlspn /*f8*/, ldsphl /*f9*/, ldaw /*fa*/, ei /*fb*/, nop /*fc*/, nop /*fd*/, cp /*fe*/, rst /*ff*/
};

void Core_init() {
    core = malloc(sizeof(struct Core));
    core->registers.sedectets.PC = 0x0000;
    core->registers.sedectets.SP = 0xFFFE;
    core->interrupts = true;
}

void Core_setHandles(void (*stop)(), void (*halt)(), struct Memory *memory) {
    core->stop = stop;
    core->halt = halt;
    core->memory = memory;
}

#if _DEBUG
    #include <stdio.h>
#endif
void Core_cycle() {
    //Handle interrupts here

#if _DEBUG
    printf("%x\n", core->registers.sedectets.PC);
#endif
    

    if (core->propagateEnableInterrupts) {
        core->propagateEnableInterrupts = false;
        core->interrupts = true;
    }

    if (core->propagateDisableInterrupts) {
        core->propagateDisableInterrupts = false;
        core->interrupts = false;
    }

    core->instruction = Memory_read(core->memory, core->registers.sedectets.PC);
    core->opcode = core->instruction & 0xFF;
    core->registers.sedectets.PC += 1;

    execute[core->opcode]();
}
