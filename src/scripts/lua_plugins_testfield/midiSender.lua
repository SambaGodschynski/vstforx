--setup
buffSize = 1024
sampleRate = 44100
gpConfig = {
   type="frx_lua_plugin", 
   name="MIDI Sender", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=0, 
   numOutputs=0
}

note=0

function sendMidi() 
   if _ENV.on==0x90 then
      _ENV.on=0x80
   else
      _ENV.on=0x90
      note=math.random(100)
   end
   msg={
      {deltaFrames=0,   data={0,0,0,0,0,0,0,0,_ENV.on, note, 100}},
   }
   frx.plug:sendMidi(msg)
end

function lcInit()
   t=frx.addTimer("sendMidi()", 30, -1)
   t:start()
end
