
require "midiHelper"

--setup
buffSize = 2048
sampleRate = 44100
gpConfig = {
   type="frx_lua_plugin", 
   name="FFT Synth", 
   author="Samba Godschynski",
   license="GPL",
   numInChannels=0, 
   numOutChannels=2
}
gpParameterSetup = {}
p = gpParameterSetup
bff = {}
phase = 0
numBand = 15
maxFrq=990
amp=0
note=0
vel=0

function key(i)
   return string.format("%02d", i)
end

function lcInit()
   initParam()
end

function lcSetAudioConfig(bs, sr)
   sampleRate = sr
   setBuffer()
end 

function initParam()
   p["frq"]=440/maxFrq
   frx.plug:addParameterListener("frq", "onFrqChanged")
   n = numBand
   for i=1,n,1 do
      name=key(i)
      p[name] = 0
      if i==3 then
	 p[name]=0.7
      end
      frx.plug:addParameterListener(name, "onBandChanged")
   end
end

function lcProcessMidi(messages) 
   for k,v in pairs(messages) do
      events=midiHelper:getMidiEvents(v.data)
      for ek, ev in pairs(events) do
	 if ev[1]==midiHelper.NOTE_ON then
	    note=ev[3][1]
	    vel=ev[3][2]
	    amp=1 --vel/127
	 elseif ev[1]==midiHelper.NOTE_OFF then
	    if note==ev[3][1] then
	       amp=0
	    end
	 end
      end
   end
end

function setFFTTable()
   r = {}
   i = {}
   n = buffSize/2
   for index=1, buffSize, 1 do
      if index < numBand then
	 r[index] = p[key(index)]
      else
	 r[index] = 0
      end
      i[index] = 0
   end
   return r, i
end

function setBuffer()
   r, i = setFFTTable()
   i, _ENV.bff = frx.plug:fft(i, r)
end

function lcProcess(numSamples)
   v={}
   for i=1, numSamples, 1 do
      v[i] = bff[math.floor(phase)+1]
      v[i] = v[i] * amp
      f=frq*(note/127)
      phase = phase + f * (buffSize/sampleRate)
      if phase > buffSize then
	 phase = 1 + phase - buffSize
      end
   end 
   frx.plug:setChannel(1, v)
   frx.plug:setChannel(2, v)
end

function onFrqChanged(name, value)
   value = value * maxFrq
   frx.plug:setParameterDisplay(name, string.format("%0.2f", value).."hz")
   _ENV.frq=value
end

function onBandChanged(name, value)
   setBuffer()
end
