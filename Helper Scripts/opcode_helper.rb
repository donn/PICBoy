# Assisting tool

class Integer
    def x
        return self >> 6
    end
    def y
        return self >> 3 & 7
    end
    def z
        return self & 7
    end
    def p
        return self.y >> 1
    end
    def q
        return self.y & 1
    end

    def hex
        self.to_s(16)
    end
end

for opcode in 0..255
    #TO-DO: Translate from CPP
    if (opcode >= 0x40) {
        nibble top = opcode >> 6;
        if (top == 0x3) {
            //SET
        } else if (top == 0x2) {
            //RESET
        } else { //if (top == 0x1) {
            //BIT
        } // else would be an impossible outcome, anything else is less than 0x40
    } else {
        switch (opcode & 0xF8) {
            case 0x38:
                //SRL n
            case 0x30:
                //SWAP
            case 0x28:
                //SRA n
            case 0x20:
                //SLA n
            case 0x18:
                //RR n
            case 0x10:
                //RL n
            case 0x08:
                //RRC n
            case 0x00:
                //RLC n
            default:
                ;//NOP
        }
    }

end