
function initBuffer(numSamples)
    res={}
    for i=1, numSamples, 1 do
    	res[i] = 0.0
    end
    return res 
end

buffer=initBuffer(44100)
cursor = 1
delay = 10000
feedback = 0.4

function incCursor()
  cursor = cursor + 1
  if cursor > delay then
     cursor = 1
  end
end


function processFrames(l, r, numSamples)
    f1 = 0.5
    f2 = 0.15
    for i=1, numSamples, 1 do
        x = l[i]
	y = buffer[cursor]
 	buffer[cursor] = x + y * feedback
	incCursor()
	l[i] = y
	r[i] = y
    end
    return l, r
end
