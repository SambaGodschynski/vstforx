--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="", 
   author="Samba Godschynski",
   license="GPL",
   numInChannels=0, 
   numOutChannels=0
}

numOut = 10 
gpParameterSetup={
   ['A/B']=0
}
aValues={}
bValues={}

p=gpParameterSetup

function lcInit()
   for i=1, numOut, 1 do
      name='param'..tostring(i)
      _ENV.gpParameterSetup[name]=0
      aValues[name]=0
      bValues[name]=1
   end
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
	 b=aValues[k]
	 e=bValues[k]
	 x=(e-b)*t+b
	 frx.plug:setParameterDisplay(k, string.format("Out: %f", x))
	 frx.plug:setParameterValue(k, x)
      end
   end
end

function lcOnParameterChanged(name, value)
   if name~="A/B" then
      ref=resetAB()
      if ref=="A" then
	 aValues[name]=value
      else
	 bValues[name]=value
      end
      frx.plug:setParameterDisplay(name, string.format("Set %s %f", ref, value))
   else
      updateParams()
   end
end



