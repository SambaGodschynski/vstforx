--setup
gpConfig = {
	 type="frx_lua_plugin", 
	 name="ADelay", 
	 author="Samba Godschynski",
	 license="GPL",
	 numInputs=5, 
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


function lcProcess(numSamples)
 --[[   l, r = frxGetFramesFromInput(1)
    for i=1, numSamples, 1 do
        x = l[i]
	y = buffer[cursor]
 	buffer[cursor] = x + y * p['feedback']
	incCursor()
	l[i] = y + p['direct'] * l[i]
	r[i] = y + p['direct'] * r[i]
    end
    frxSetFramesToOutput(1, l, r)]]

end

function lcOnParameterChanged(name, value)
    if name=='delay' then
       frxLog(delay)
       value = value * maxbuff
    end
    p[name]=value
end