--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="ADelay", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=2, 
   numOutputs=2
}


gpParameterSetup={direct=0.5, delay=0.5, feedback=0.4}
p=gpParameterSetup

plug=frx.plug
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
   if cursor > p['delay']:getValue() * maxbuff then
      cursor = 1
   end
end


function lcProcess(numSamples)
   l = plug:getInput(1)
   r = plug:getInput(2)
   for i=1, numSamples, 1 do
      x = l[i]
      y = buffer[cursor]
      buffer[cursor] = x + y * p['feedback']:getValue()
      incCursor()
      l[i] = y + p['direct']:getValue() * l[i]
      r[i] = y + p['direct']:getValue() * r[i]
   end
   frx.plug:toOutput(1, l)
   frx.plug:toOutput(2, r)
end


function lcOnParameterChanged(x)
   --if name=='delay' then
     --value = value * maxbuff
   --end
   print(x)
end
