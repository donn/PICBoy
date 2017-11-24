extension UInt8 {
    var x: UInt8 {
        return self >> 6
    }
    var y: UInt8 {
        return self >> 3 & 7
    }
    var z: UInt8 {
        return self & 7
    }
    var p: UInt8 {
        return self.y >> 1
    }
    var q: UInt8 {
        return self.y & 1
    }

    var hex: String {
        return String(self, radix: 16)
    }
}

for j in 0...255 {
    let i = UInt8(j)
    if i.x == 3 && i.z == 6 {
        //print(String(i, radix: 16), terminator: ", ")
        print(i.hex)
    }
}