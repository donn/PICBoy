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

for i in 0...256
     if i.x == 3 && i.z == 0
         puts i.hex
    end
end