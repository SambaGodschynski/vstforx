--setup
gpParameterSetup = { direct = 0.5, delay=0.5, feedback=0.4 }
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
delay = 10000
feedback = 0.8
direct = 0.5

function incCursor()
  cursor = cursor + 1
  if cursor > delay then
     cursor = 1
  end
end


function lcProcess(numSamples)
    l, r = frxGetFramesFromInput(1)
    for i=1, numSamples, 1 do
        x = l[i]
	y = buffer[cursor]
 	buffer[cursor] = x + y * feedback
	incCursor()
	l[i] = y + direct * l[i]
	r[i] = y + direct * r[i]
    end
    frxSetFramesToOutput(1, l, r)
end

function lcOnParameterChanged(name, value)
    if name=='feedback' then
        feedback = value
    end
    if name=='delay' then
       delay = value * maxbuff
    end
    if name=='direct' then
       direct = value
    end
end