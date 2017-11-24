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

static void jrc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void jr(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void stop(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
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

static void rlca(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void rrca(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void rla(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void rra(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void daa(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void cpl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void scf(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ccf(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}


static void ld(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void halt(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}


static void add(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void adc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void sub(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void sbc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void and(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void xor(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void or(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void cp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}


static void retc(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}

static void pop(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ret(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldff00bytea(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) { 
}
static void ldaff00byte(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldhlsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void addsp(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void jphl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void reti(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
}
static void ldsphl(struct Core* core, uint32 instruction, byte x, byte y, byte z, byte p, byte q) {
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

/*
    nop_0, ldi16_1, ild_2, inc16_3, inc8_4, dec8_5, ldi8_6, rlca_7
    ldsp_8, add16_9, ild_a, dec16_b, inc8_c, dec8_d, ldi8_e, rrca_f
    stop_10, ldi16_11, ild_12, inc16_13, inc8_14, dec8_15, ldi8_16, rla_17
    jr_18, add16_19, ild_1a, dec16_1b, inc8_1c, dec8_1d, ldi8_1e, rra_1f
    jrc_20, ldi16_21, ildm_22, inc16_23, inc8_24, dec8_25, ldi8_26, daa_27
    jrc_28, add16_29, ildm_2a, dec16_2b, inc8_2c, dec8_2d, ldi8_2e, cpl_2f
    jrc_30, ldi16_31, ildm_32, inc16_33, inc8_34, dec8_35, ldi8_36, scf_37
    jrc_38, add16_39, ildm_3a, dec16_3b, inc8_3c, dec8_3d, ldi8_3e, ccf_3f
    ld_40, ld_41, ld_42, ld_43, ld_44, ld_45, ld_46, ld_47
    ld_48, ld_49, ld_4a, ld_4b, ld_4c, ld_4d, ld_4e, ld_4f
    ld_50, ld_51, ld_52, ld_53, ld_54, ld_55, ld_56, ld_57
    ld_58, ld_59, ld_5a, ld_5b, ld_5c, ld_5d, ld_5e, ld_5f
    ld_60, ld_61, ld_62, ld_63, ld_64, ld_65, ld_66, ld_67
    ld_68, ld_69, ld_6a, ld_6b, ld_6c, ld_6d, ld_6e, ld_6f
    ld_70, ld_71, ld_72, ld_73, ld_74, ld_75, halt_76, ld_77
    ld_78, ld_79, ld_7a, ld_7b, ld_7c, ld_7d, ld_7e, ld_7f
    add_80, add_81, add_82, add_83, add_84, add_85, add_86, add_87
    adc_88, adc_89, adc_8a, adc_8b, adc_8c, adc_8d, adc_8e, adc_8f
    sub_90, sub_91, sub_92, sub_93, sub_94, sub_95, sub_96, sub_97
    sbc_98, sbc_99, sbc_9a, sbc_9b, sbc_9c, sbc_9d, sbc_9e, sbc_9f
    and_a0, and_a1, and_a2, and_a3, and_a4, and_a5, and_a6, and_a7
    xor_a8, xor_a9, xor_aa, xor_ab, xor_ac, xor_ad, xor_ae, xor_af
    or_b0, or_b1, or_b2, or_b3, or_b4, or_b5, or_b6, or_b7
    cp_b8, cp_b9, cp_ba, cp_bb, cp_bc, cp_bd, cp_be, cp_bf
    ret_c0, pop_c1, jpc_c2, jp_c3, callc_c4, push_c5, addi_c6, rst_c7
    ret_c8, ret_c9, jpc_ca, cb_cb, callc_cc, call_cd, adci_ce, rst_cf
    ret_d0, pop_d1, jpc_d2, nop_d3, callc_d4, push_d5, subi_d6, rst_d7
    ret_d8, reti_d9, jpc_da, nop_db, callc_dc, nop_dd, sbci_de, rst_df
    ret_e0, pop_e1, ldff00ca_e2, nop_e3, nop_e4, push_e5, andi_e6, rst_e7
    ret_e8, jphl_e9, ldwa_ea, nop_eb, nop_ec, nop_ed, xori_ee, rst_ef
    ret_f0, pop_f1, nop_f2, di_f3, nop_f4, push_f5, ori_f6, rst_f7
    ret_f8, ldsphl_f9, ldaw_fa, ei_fb, nop_fc, nop_fd, cpi_fe, rst_ff
*/
void (*execute[])(struct Core*, uint32, byte, byte, byte, byte, byte) = {
    nop, ldi16, ild, inc16, inc8, dec8, ldi8, rlca, 
    ldsp, add16, ild, dec16, inc8, dec8, ldi8, rrca, 
    stop, ldi16, ild, inc16, inc8, dec8, ldi8, rla, 
    jr, add16, ild, dec16, inc8, dec8, ldi8, rra, 
    jrc, ldi16, ildm, inc16, inc8, dec8, ldi8, daa, 
    jrc, add16, ildm, dec16, inc8, dec8, ldi8, cpl, 
    jrc, ldi16, ildm, inc16, inc8, dec8, ldi8, scf, 
    jrc, add16, ildm, dec16, inc8, dec8, ldi8, ccf, 
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
    ret, pop, jpc, nop, callc, push, subi, rst, 
    ret, reti, jpc, nop, callc, nop, sbci, rst, 
    ret, pop, ldff00ca, nop, nop, push, andi, rst, 
    ret, jphl, ldwa, nop, nop, nop, xori, rst, 
    ret, pop, nop, di, nop, push, ori, rst, 
    ret, ldsphl, ldaw, ei, nop, nop, cpi, rst
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
