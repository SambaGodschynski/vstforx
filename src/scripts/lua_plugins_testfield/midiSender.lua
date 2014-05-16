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

function sendMidi() 
   if _ENV.on==0x91 then
      _ENV.on=0x81
   else
      _ENV.on=0x91
   end
   msg={
      {deltaFrames=1,   data={_ENV.on, 61, 120}},
   }
   frx.plug:sendMidi(msg)
end

function lcInit()
   t=frx.addTimer("sendMidi()", 2000, -1)
   t:start()
end
