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
   t = frx.addTimer("updateParameter()", 50, -1)
   t:start()
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
      frx.plug:setParameterDisplay(name, string.format("%.02f minutes",value))
   end
end


function updateSamplePos()
   s = frx.plug:getSamplePos()
   s = s / (sampleRate * 60 * p['totalSamples'] * maxSampleMinutes)
   s = math.min(s, 1)
   frx.plug:setParameterValue("samplePos", s)
   frx.plug:setParameterDisplay("samplePos", string.format("%.02f minutes", s))
end

function updateBarPos()
   ppq = frx.plug:getPpqPos()
   denom = frx.plug:getTimeSigDenominator()
   num = frx.plug:getTimeSigNumerator()
   x = (ppq/4) / (1/denom)
   step = (math.floor(x) % num) / num
   _float = (x / num) % 1
   frx.plug:setParameterValue("barPosFloat", _float)
   frx.plug:setParameterValue("barPosStep", step)
end

function updateParameter()
   updateSamplePos()
   updateBarPos()
   frx.plug:log("hi")
end
