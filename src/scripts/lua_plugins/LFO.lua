-----------------------------------------------------------------
-- VSTForx.lua LFO plugin                                      --
-- A simple LFO for parameter                                  --
-- a VSTForx.Lua documentation can be found under:             --
--  1. Overview............http://api.vstforx.de               --
--  2. Lua Plugin Doc......http://api.vstforx.de/LuaPlugin     --
-----------------------------------------------------------------

--setup table, will be loaded when initalizing plugin
gpConfig = {
   type="frx_lua_plugin", 
   name="LFO", 
   author="Samba Godschynski",
   numInChannels=0, 
   numOutChannels=0,
   info="A simple parameter LFO"
}

--Parameter setup table, will be loaded when initalizing plugin.
--When a parameter is changed this map will be updated, so you can
--use 'gpParameterSetup[name]' for reading a parameter value.
--This map is READ only, writing values will have no affect.
--Use frx.plug:setParameterValue(name, value) instead.
gpParameterSetup = {
   ["0_OUT_SIN"]=0,
   ["0_OUT_SQR"]=0,
   ["1_FRQ"]=0.01,
   ["2_SYNC"]=0,
   ["AMP"]=1,
   ["OFFSET"]=0
}

sync=false
frq=1
maxFrq=5
--adding parameter listeners 
frx.plug:addParameterListener("2_SYNC", "onSyncChanged");
frx.plug:addParameterListener("1_FRQ", "onFrqChanged");

--will be called at startup
function lcInit()
   --add a timer which updates the out parameters
   _ENV.TIMER = frx.addTimer(
      "onTimer()", -- the callback expression 
      1,          -- the timer time in milliseconds
      -1           -- reputation, -1 means infinite
   )
   _ENV.TIMER:start() -- now start the timer
end

--FRQ parameter changed
function onFrqChanged(name, value)
   if sync==false then
      frq=maxFrq*value
      frx.plug:setParameterDisplay("1_FRQ", string.format("%0.2fhz", frq))
      return
   end
   value=math.floor(value*5-2)
   frq=math.pow(2, value)
   frx.plug:setParameterDisplay("1_FRQ", string.format("x%0.2f", frq))
end

--SYNC parameter changed
function onSyncChanged(name, value)
   if value>0.5 then
      frx.plug:setParameterDisplay("2_SYNC","YES")
      sync=true
   else
      frx.plug:setParameterDisplay("2_SYNC", "NO")
      sync=false
   end
   --update frq
   frx.plug:setParameterValue("1_FRQ", gpParameterSetup['1_FRQ'])
end

--determine whether DAW is playing 
function isPlaying()
   return frx.plug:transportIsPlaying()
end

--update SIN parameter
function updateSinValue(x, _isPlaying)
   x = math.abs(x) * gpParameterSetup['AMP'] + gpParameterSetup['OFFSET']
   x = math.min(x,1)
   if not _isPlaying and sync==true then
      frx.plug:setParameterDisplay("0_OUT_SIN", "not playing")
   else
      frx.plug:setParameterDisplay("0_OUT_SIN", "")
      frx.plug:setParameterValue("0_OUT_SIN", x)
   end
end

--update SQR parameter
function updateSqrValue(x, _isPlaying)
   x=math.abs(x)
   if x>0.5 then
      x=1
   else
      x=0
   end
   x = x * gpParameterSetup['AMP'] + gpParameterSetup['OFFSET']
   x = math.min(x,1)
   if not _isPlaying and sync==true then
      frx.plug:setParameterDisplay("0_OUT_SQR", "not playing")
   else
      frx.plug:setParameterDisplay("0_OUT_SQR", "")
      frx.plug:setParameterValue("0_OUT_SQR", x)
   end
end


--get t value
function getT()
   if sync==false then
      t = os.clock()
      return t
   end
   t=frx.plug:getPpqPos()
   return t
end

--timer callback
function onTimer()
   p = isPlaying()
   x = math.sin(getT()*math.pi*frq)
   updateSinValue(x, p)
   updateSqrValue(x, p)
end
