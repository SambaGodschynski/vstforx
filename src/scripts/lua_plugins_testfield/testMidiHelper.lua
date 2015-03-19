require "midiHelper"


testData = {
   { deltaFrames=0, data={0,0,0,0,0,0,0x91,0x99, 0x10, 0, 0, 0} },
   { deltaFrames=0, data={0,0,0,0,0,0,0x81,0x99, 0x0, 0, 0, 0, 0, 0} },
   { deltaFrames=0, data={0,0,0,0xC2,0x81, 0, 0, 0} },
   { deltaFrames=0, data={0xE3,0x100} },
   { deltaFrames=0, data={0x99, 0xF0, 0, 0, 0} },
   { deltaFrames=0, data={0xF7, 0xF0, 0xF7} }
}

it = midi.Iterator:new(testData)
while it:next() do
   print(it:type(), it:channel(), it:data1(), it:data2()) 
end
