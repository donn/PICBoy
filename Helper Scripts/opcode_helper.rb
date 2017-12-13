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

cbOpcodeArray = "byte cbExecute[] = {"

for opcode in 0..255
    if opcode % 8 == 0
        cbOpcodeArray += "\n   "
    end
    #TO-DO: Translate from CPP
    if opcode >= 0x40 
        top = opcode >> 6;
        if (top == 0x3)
            cbOpcodeArray += " setbit /*%02x*/," % opcode
        elsif (top == 0x2)
            cbOpcodeArray +=  " resetbit /*%02x*/," % opcode
        else 
            cbOpcodeArray +=  " testbit /*%02x*/," % opcode
        end
    else 
        case (opcode & 0xF8)
        when 0x38
            cbOpcodeArray +=  " srln /*%02x*/," % opcode
        when 0x30
            cbOpcodeArray +=  " swap /*%02x*/," % opcode
        when 0x28
            cbOpcodeArray +=  " sran /*%02x*/," % opcode
        when 0x20
            cbOpcodeArray +=  " slan /*%02x*/," % opcode
        when 0x18
            cbOpcodeArray +=  " rrn /*%02x*/," % opcode
        when 0x10
            cbOpcodeArray +=  " rln /*%02x*/," % opcode
        when 0x08
            cbOpcodeArray +=  " rrcn /*%02x*/," % opcode
        when 0x00
            cbOpcodeArray +=  " rlcn /*%02x*/," % opcode
        end
    end
end

cbOpcodeArray.chomp!
cbOpcodeArray += "\n};"

puts cbOpcodeArray
