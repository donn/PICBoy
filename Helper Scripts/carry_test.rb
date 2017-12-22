# Unfortunately this failed miserably because I forgot how multiplication works

for i in 0..255
    for j in 0..255
        ans = 0
        h = 0
        c = 0
        altans = 0
        alth = 0
        altc = 0

        ans = (i + j + 1) & 0xFF
        if ans & 0xF < i & 0xF
            h = 1
        end
        if ans < i
            c = 1
        end

        altans = (i + j) & 0xFF

        altans = (altans + 1) & 0xFF
        if altans & 0xF < i & 0xF
            alth = 1
        end
        if altans < i
            altc = 1
        end

        if ans != altans || h != alth || c != altc
            puts "#{i} #{j} failed: #{ans}, #{altans}, #{h}, #{c}, #{alth}, #{altc}"
        end

        # None fail: apparently this works!
    end
end

anstable = []
flagtable = []

for i in 0..255
    for j in 0..255
        ans = 0
        h = 0
        c = 0
        altans = 0
        alth = 0
        altc = 0

        ans = (i + j + 1) & 0xFF
        if ans & 0xF < i & 0xF
            h = 1
        end
        if ans < i
            c = 1
        end
        
        z = ans==0? 1: 0;
        n = 0;

        flags = 0 | z << 7 | n << 6 | h << 5 | c << 4

        anstable << ans
        flagtable << flags

    end
end

flagArray = "byte flags[] = {"

flagtable.each_with_index do
    |flags, index|
    if index % 8 == 0
        flagArray += "\n"
    end
    flagArray += " 0x%02x," % [flags]
end

flagArray.chomp! ","
flagArray += "\n}"
puts flagArray

ansArray = "byte results[] = {"

anstable.each_with_index do
    |ans, index|
    if index % 8 == 0
        ansArray += "\n"
    end
    ansArray += " 0x%02x," % [ans]
end

ansArray.chomp! ","
ansArray += "\n}"
puts ansArray