--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="Musical Values", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=0, 
   numOutputs=0
}
gpParameterSetup = {
   totalSamples=1,
   samplePos=0,
   barPosFloat=0,
   barPosStep=0
}
p = gpParameterSetup
sampleRate = 44100
maxSampleMinutes = 5

function lcInit()
   frxAddTimer("onTimer", 50, -1)
end

function lcSetAudioConfig(bs, sr)
   if sr==0 then
      return
   end
   sampleRate = sr
end

function lcOnParameterChanged(name, value)
   if name == "totalSamples" then
      value = value * maxSampleMinutes
      frxSetParameterDisplay(name, string.format("%.02f minutes",value))
   end
end


function updateSamplePos()
   s = frxGetSamplePos()
   s = s / (sampleRate * 60 * p['totalSamples'] * maxSampleMinutes)
   s = math.min(s, 1)
   frxSetParameterValue("samplePos", s)
   frxSetParameterDisplay("samplePos", string.format("%.02f minutes", s))
end

function updateBarPos()
   ppq = frxGetPpqPos()
   denom = frxGetTimeSigDenominator()
   num = frxGetTimeSigNumerator()
   x = (ppq/4) / (1/denom)
   step = (math.floor(x) % num) / num
   _float = (x / num) % 1
   frxSetParameterValue("barPosFloat", _float)
   frxSetParameterValue("barPosStep", step)
end

function onTimer()
   updateSamplePos()
   updateBarPos()
end
