--setup
buffSize = 1024
sampleRate = 44100
gpParameterSetup = {}
p = gpParameterSetup
bff = {}
phase = 0
numBand = 15
maxFrq=990

function key(i)
         return string.format("%02d", i)
end

function lcInit()
	 _ENV.gpParameterSetup = initParam()
	 setBuffer()
end

function initParam()
	 res={frq=440/maxFrq}
	 n = numBand
	 for i=1,n,1 do
	     res[key(i)] = 0
	 end
	 return res
end

function setFFTTable()
	 r = {}
	 i = {}
	 n = buffSize/2
	 for index=1, buffSize, 1 do
	     if index < numBand then
	     	r[index] = p[key(index)]
	     else
		r[index] = 0
	     end
	     i[index] = 0
	 end
	 return r, i
end

function setBuffer()
	 r, i = setFFTTable()
	 i, _ENV.bff = frxFFT(i, r, buffSize)
end

function lcProcess(numSamples)
	 v={}
	 for i=1, numSamples, 1 do
	     v[i] = bff[math.floor(phase)]
	     phase = phase + p['frq'] * (buffSize/sampleRate)
	     if phase > buffSize then
	     	phase = 1 + phase - buffSize
	     end
	 end
	 frxSetFramesToOutput(1, v, v)
end

function lcOnParameterChanged(name, value)
	 if name=='frq' then
	    value = value * maxFrq
	 end
	 p[name] = value
	 setBuffer()
end