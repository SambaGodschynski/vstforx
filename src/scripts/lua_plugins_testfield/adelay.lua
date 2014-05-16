--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="ADelay", 
   author="Samba Godschynski",
   license="GPL",
   numInChannels=2, 
   numOutChannels=2
}
gpParameterSetup = { direct = 0.5, delay=0.5, feedback=0.4 }
p = gpParameterSetup
maxbuff = 44100


function initBuffer(numSamples)
   res={}
   for i=1, numSamples, 1 do
      res[i] = 0.0
   end
   return res
end
buffer=initBuffer(maxbuff)

cursor = 1

function incCursor()
   cursor = cursor + 1
   if cursor > p['delay'] then
      cursor = 1
   end
end

t=frx.addTimer("frx.plug:log('ARSCH')", 50, -1)
t:start()

function lcProcess(numSamples)
   l = frx.plug:getChannel(1)
   r = frx.plug:getChannel(2)
   for i=1, numSamples, 1 do
      x = l[i]
      y = buffer[cursor]
      buffer[cursor] = x + y * p['feedback']
      incCursor()
      l[i] = y + p['direct'] * l[i]
      r[i] = y + p['direct'] * r[i]
   end
   frx.plug:setChannel(1, l)
   frx.plug:setChannel(2, r)
end

function lcOnParameterChanged(name,value)
   if name=='delay' then
      value = value * maxbuff
   end
   p[name]=value
end
