-----------------------------------------------------------------
-- VSTForx.lua SimpleDelay plugin                              --
-- A simple Delay for demo purpose                             --
-- a VSTForx.Lua documentation can be found under:             --
--  1. Overview............http://api.vstforx.de               --
--  2. Lua Plugin Doc......http://api.vstforx.de/LuaPlugin     --
-----------------------------------------------------------------

--setup table, will be loaded when initalizing plugin
gpConfig = {
   type="frx_lua_plugin", 
   name="Simple Delay", 
   author="Samba Godschynski",
   numInChannels=2, 
   numOutChannels=2,
   info="A simple delay to show how a VSTForx.lua plugin works."
}
--Parameter setup table, will be loaded when initalizing plugin.
--When a parameter is changed this map will be updated, so you can
--use 'gpParameterSetup[name]' for reading a parameter value.
--This map is READ only, writing values will have no affect.
--Use frx.plug:setParameterValue(name, value) instead.
gpParameterSetup = { 
   direct = 0.5, 
   delay=0.5, 
   feedback=0.4
}
--a handy abbreviation for gpParameterSetup
p=gpParameterSetup


buffer={}
maxbuff=0

function initBuffer(numSamples)
   res={}
   for i=1, numSamples, 1 do
      res[i] = 0.0
   end
   return res
end

--will be called when audio config changed
function lcSetAudioConfig(blockSize, sampleRate)
   --max delay = 1 sec. = sampleRate
   _ENV.buffer = initBuffer(sampleRate)
   _ENV.maxbuff = sampleRate
end

cursor = 1

function incCursor()
   cursor = cursor + 1
   delay = p['delay'] * maxbuff
   if cursor > delay then
      cursor = 1
   end
end

--this is the process callback
function lcProcess(numSamples)
   --get the input data
   l = frx.plug:getChannel(1) --left channel
   r = frx.plug:getChannel(2) --right channel
   --apply delay
   for i=1, numSamples, 1 do
      x = l[i]
      y = buffer[cursor]
      buffer[cursor] = x + y * p['feedback']
      incCursor()
      l[i] = y + p['direct'] * l[i]
      r[i] = y + p['direct'] * r[i]
   end
   --set the resulting data
   frx.plug:setChannel(1, l)
   frx.plug:setChannel(2, r)
end

