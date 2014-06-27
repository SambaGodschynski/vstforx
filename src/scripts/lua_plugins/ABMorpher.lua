-----------------------------------------------------------------
-- VSTForx.lua ABMorpher plugin                                --
-- a VSTForx.Lua documentation can be found under:             --
--  1. Overview............http://api.vstforx.de               --
--  2. Lua Plugin Doc......http://api.vstforx.de/LuaPlugin     --
-----------------------------------------------------------------
--setup table, will be loaded when initalizing plugin
gpConfig = {
   type="frx_lua_plugin", 
   name="", 
   author="Samba Godschynski",
   numInChannels=0, 
   numOutChannels=0,
   info="This plugin manages parameter transitions between \
two states: A and B."
}

numOut = 10 -- change this to have more or less parameter

--Parameter setup table, will be loaded when initalizing plugin.
--When a parameter is changed this map will be updated, so you can
--use 'gpParameterSetup[name]' for reading a parameter value.
--This map is READ only, writing values will have no affect.
--Use frx.plug:setParameterValue(name, value) instead.
gpParameterSetup={
   ['A/B']=0
}

values={}

p=gpParameterSetup

function lcInit()
   --when loaded with arguments
   if _ENV.__args ~= nil then
      if _ENV.__args['numParams'] ~= nil then
	 numOut = _ENV.__args['numParams']
      end
   end
   --init parameter
   for i=1, numOut, 1 do
      name=string.format('param%03i', i)
      _ENV.gpParameterSetup[name]=0
      values[name]={0, nil}
      frx.plug:addParameterListener(name, "onParameterChanged")
   end
   frx.plug:addParameterListener("A/B", "onABChanged")
end


function resetAB()
   -- set AB either to A or B
   if p['A/B']>0.5 then
      frx.plug:setParameterValue("A/B", 1)
      return "B"
   end
   frx.plug:setParameterValue("A/B", 0)
   return "A"
end

function updateParams()
   for k,e in pairs(p) do
      if k~="A/B" then
	 t=p["A/B"]
	 b=values[k][1]
	 e=values[k][2]
	 if e==nil then
	    e=b
	 end
	 x=(e-b)*t+b
	 frx.plug:setParameterDisplay(k, string.format("Out: %f", x))
	 frx.plug:setParameterValue(k, x)
      end
   end
end

blockParams=false

function onABChanged()
   blockParams=true
   updateParams()
   blockParams=false
end

function onParameterChanged(name, value)
   if blockParams==true then
      return
   end
   currState=resetAB()
   if currState=="A" then
      values[name][1]=value
   else
      values[name][2]=value
   end
   frx.plug:setParameterDisplay(name, string.format("Set %s %f", currState, value))
end

function lcOnSave()
   -- save AB data
   for k, v in pairs(values) do
      local data={v[1], v[2]}
      if data[2]==nil then -- #2 can be nil which will not stored
	 data[2]=-1
      end
      -- cause of http://issues.vstforx.de/view.php?id=488 we 
      -- save every value separately 
      frx.plug:setPersistUserData(k.."a", {data[1]})
      frx.plug:setPersistUserData(k.."b", {data[2]})
   end
   frx.plug:log("saved")
end

function lcOnLoad()
   -- load and set AB data
   for k, v in pairs(_ENV.values) do
      local dataA = frx.plug:getPersistUserData(k.."a")
      local dataB = frx.plug:getPersistUserData(k.."b")
      if #dataA==1 and #dataB==1 then
	 dataA=tonumber(dataA[1])
	 dataB=tonumber(dataB[1])
	 _ENV.values[k][1]=dataA
	 _ENV.values[k][2] = (dataB>=0) and dataB or nil 
      else
	 frx.plug:logErr(string.format("loading failed: no data for %s", k))
      end
   end
end


