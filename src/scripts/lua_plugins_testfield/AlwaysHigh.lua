--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="", 
   author="Samba Godschynski",
   numInChannels=0, 
   numOutChannels=0,
   info="The output parameter reacts only when turning up the input parameter."
}

gpParameterSetup={
   input=0,
   output=0,
   mode=0
}


p=gpParameterSetup

function lcInit()
   frx.plug:addParameterListener("input", "onInputChanged")
   frx.plug:addParameterListener("mode", "onModeChanged")
end

oldValue=0
mode="cyclic"
dir=1

function updateOutput(x)
   o=frx.plug:getParameterValue("output")
   if mode=="cyclic" then
      o=(o+x)%1
   elseif mode=="pingpong" then
      if o >= 1 then
	 dir=-1
      elseif o<=0 then
	 dir=1
      end
      if dir>0 then
	 o=math.min(o+x, 1)
      else
	 o=math.max(o-x,0)
      end
   elseif mode=="final" then
      o=math.min(o+x, 1)
   end
   frx.plug:setParameterValue("output", o)
end

function onInputChanged(name, value)
   diff = value-oldValue
   if diff > 0 then
      updateOutput(diff)
   end
   oldValue=value
end

function onModeChanged(name, value)
   s = math.floor(3 * math.min(value,0.99999))
   if s==0 then
      mode="cyclic"
   elseif s==1 then
      mode="pingpong"
   elseif s==2 then
      mode="final"
   end
   frx.plug:setParameterDisplay("mode", mode)
end


