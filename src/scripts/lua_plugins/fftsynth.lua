--setup
buffSize = 256
sampleRate = 44100
gpConfig = {
   type="frx_lua_plugin", 
   name="FFT Synth", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=0, 
   numOutputs=2
}
gpParameterSetup = {}
p = gpParameterSetup
bff = {}
phase = 0
numBand = 15
maxFrq=990

function key(i)
   return string.format("%02d", i)
end

function lcInit()
   initParam()
end

function lcSetAudioConfig(bs, sr)
   sampleRate = sr
   setBuffer()
   frx.log("audioConfig set")
end 

function initParam()
   p["frq"]=440/maxFrq
   n = numBand
   for i=1,n,1 do
      p[key(i)] = 0
      if i==3 then
	 p[key(i)]=0.7
      end
   end
end

function lcProcessMidi(messages) 
   for k,v in pairs(messages) do
      print(#v.data)
      _status, v1, v2 = unpack(v.data)
      print(_status, v1, v2)
      if (_status~=nil) then 
	 status=bit32.band(0xF0, _status)
	 channel=bit32.band(0x0F, _status)
	 if status==0x80 then
	    --noteoff
	    frx.plug:log(channel, "note off")
	 end
	 if status==0x90 then
	    --noteon
	    frx.plug:log(channel, "note on")
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
      v[i] = bff[math.floor(phase)]
      phase = phase + p['frq'] * (buffSize/sampleRate)
      if phase > buffSize then
	 phase = 1 + phase - buffSize
      end
   end 
   frx.plug:toOutput(1, v)
   frx.plug:toOutput(2, v)
end

function lcOnParameterChanged(name, value)
   if name=='frq' then
      value = value * maxFrq
      frx.plug:setParameterDisplay(name, string.format("%0.2f", value).."hz")
   end
   p[name] = value
   setBuffer()
end
