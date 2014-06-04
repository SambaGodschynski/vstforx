-----------------------------------------------------------------
-- VSTForx.lua SimpleGain plugin                               --
-- A simple Gain for demo purpose                              --
-- a VSTForx.Lua documentation can be found under:             --
--  1. Overview............http://api.vstforx.de               --
--  2. Lua Plugin Doc......http://api.vstforx.de/LuaPlugin     --
-----------------------------------------------------------------

--setup table, will be loaded when initalizing plugin
gpConfig = {
   type="frx_lua_plugin", 
   name="Simple Gain", 
   author="Samba Godschynski",
   numInChannels=2, 
   numOutChannels=2,
   info="A simple gain to demonstrate how a VSTForx.lua plugin works."
}
--Parameter setup table, will be loaded when initalizing plugin.
--When a parameter is changed this map will be updated, so you can
--use 'gpParameterSetup[name]' for reading a parameter value.
--This map is READ only, writing values will have no affect.
--Use frx.plug:setParameterValue(name, value) instead.
gpParameterSetup = { 
   gain = 0.5
}
--a handy abbreviation for gpParameterSetup
p=gpParameterSetup

maxRange=5
gain=0

--add a parameter listener for gain
frx.plug:addParameterListener("gain", "onGainChanged")
function onGainChanged(name, value)
   --we want a value range between 0..maxRange
   gain=value*maxRange
   --parameters can only handle values between 0..1
   --but we can set the parameter display
   str=string.format("x%0.2f", gain) --we want only 2 digits of precision
   frx.plug:setParameterDisplay("gain", str)
end


--this is the process callback
function lcProcess(numSamples)
   --get the input data
   l = frx.plug:getChannel(1) --left channel
   r = frx.plug:getChannel(2) --right channel
   --apply gain
   for i=1, numSamples, 1 do
      l[i] = math.min(1, l[i] * gain) 
      r[i] = math.min(1, l[i] * gain)
   end
   --set the resulting data
   frx.plug:setChannel(1, l)
   frx.plug:setChannel(2, r)
end

