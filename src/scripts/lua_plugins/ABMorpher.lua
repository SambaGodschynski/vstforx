--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="", 
   author="Samba Godschynski",
   numInChannels=0, 
   numOutChannels=0,
   info="This plugin manages parameter transitions between \
two states: A and B."
}

numOut = 100 -- change this to have more or less parameter
gpParameterSetup={
   ['A/B']=0
}

values={}

p=gpParameterSetup

function lcInit()
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



