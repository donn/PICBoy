#!/usr/bin/env ruby

# Unpack gameboy games to a C array

File.open("Game.tcc", "w") do |file|
    file << File.read(ARGV[0]).unpack("C*").map {|byte| "#{byte}," }.join("\n")
    #Append three bytes to help with 32-bit accesses
    file.puts "\n0,\n0,\n0"
end