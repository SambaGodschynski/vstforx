--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="ADelay", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=2, 
   numOutputs=2
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

function lcOnSave()
   frx.plug:setPersistUserData("userkey", {"hallo", "du", "penner"})
end

function lcOnLoad()
   data = frx.plug:getPersistUserData("userkey")
   for k,v in pairs(data) do
      print (k,v)
   end
end

function lcProcess(numSamples)
   l = frx.plug:getInput(1)
   r = frx.plug:getInput(2)
   for i=1, numSamples, 1 do
      x = l[i]
      y = buffer[cursor]
      buffer[cursor] = x + y * p['feedback']
      incCursor()
      l[i] = y + p['direct'] * l[i]
      r[i] = y + p['direct'] * r[i]
   end
   frx.plug:toOutput(1, l)
   frx.plug:toOutput(2, r)
end

function lcOnParameterChanged(name,value)
   if name=='delay' then
      value = value * maxbuff
   end
   p[name]=value
end
