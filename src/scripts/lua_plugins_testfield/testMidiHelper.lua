require "midiHelper"

--[[
      UNKNOWN=0,
      NOTE_ON=0x9,
      NOTE_OFF=0x8,
      POLY_AFTERTOUCH=0xA,
      CC=0xB,
      PC=0xC,
      MONO_AFTERTOUCH=0xD,
      PITCH_BEND=0xE,
      SYSEX=0xF,
]]

testData = {
   { deltaFrames=0, data={0,0,0,0,0,0,0x91,0x99, 0x10, 0, 0, 0} },
   { deltaFrames=0, data={0,0,0,0,0,0,0x81,0x99, 0x0, 0, 0, 0, 0, 0} },
   { deltaFrames=0, data={0,0,0,0xC2,0x81, 0, 0, 0} },
   { deltaFrames=0, data={0xE3,127} },
   { deltaFrames=0, data={127, 0xF0, 0, 1, 2} },
   { deltaFrames=0, data={0xF7, 0xF0, 0xF7, 0xF0} }
}

--print(string.format("%x", bit32.rshift(0xF1, 4)))

it = midi.Iterator:new(testData)
while it:next() do
   print(midi.typeToString(it.type), it.channel, it:pitch(), it:velocity(), it:pitchBend(), it.size)
end
