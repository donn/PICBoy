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

puts 0x10.x, 0x10.y, 0x10.z