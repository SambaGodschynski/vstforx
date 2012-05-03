--setup
buffSize = 256
sampleRate = 44100
gpParameterSetup = {}
numBand = 15
p = gpParameterSetup
bff = {}
fftI = {}
cursor = 0

function initParam()
         for i=1, numBand, 1 do
             _ENV.gpParameterSetup[tostring(i)] = 0
         end
end

function lcInit()
         for i=1, buffSize, 1 do
             _ENV.bff[i] = 0
             _ENV.fftI[i] = 0
         end
         initParam()
end

function updateParam()
      _r, _i = frxFFT(bff, fftI, buffSize)
      for i=1, numBand, 1 do
          if i > buffSize then
             break
          end
          --R = _r[i] / buffSize
          --I = _i[i] / buffSize
          v = _r[i] --math.sqrt( R*R + I*I )
          frxSetParameterValue(tostring(i), v)
      end
end

function incCursor()
         cursor = cursor + 1
         if cursor > buffSize then
            cursor = 1
            updateParam()
         end
end

function lcProcess(numSamples)
         l, r = frxGetFramesFromInput(0)
         for i=1, numSamples, 1 do
             bff[cursor] = math.abs( l[i] / 2 + r[i] / 2 )
             incCursor()
             l[i] = 0
             r[i] = 0
         end
         frxSetFramesToOutput(1, l, r)
end
