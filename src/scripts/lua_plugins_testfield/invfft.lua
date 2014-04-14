--setup
buffSize = 256
sampleRate = 44100
gpParameterSetup = {}
numBand = 15
p = gpParameterSetup
bff = {}
fftI = {}
cursor = 0

function key(i)
         return string.format("%02d", i)
end

function initParam()
         for i=1, numBand, 1 do
             _ENV.gpParameterSetup[key(i)] = 0
         end
end

function lcInit()
         for i=1, buffSize, 1 do
             _ENV.bff[i] = 0
             _ENV.fftI[i] = 0
         end
         initParam()
end

function updateBuffer()
      for i=1, buffSize, 1 do
          if i >= numBand then
             bff[i] = 0
          else
              bff[i] = p[key(i)]
          end
      end
      _i, bff = frxFFT(fftI, bff, buffSize)
end

function incCursor()
         cursor = cursor + 1
         if cursor > buffSize then
            cursor = 1
            updateBuffer()
         end
end

function lcProcess(numSamples)
         l = {}
         r = {}
         for i=1, numSamples, 1 do
             l[i] = bff[cursor]
             r[i] = bff[cursor]
             incCursor()
         end
         frxSetFramesToOutput(1, l, r)
end

function lcOnParameterChanged(name, value)
         p[name] = value
end
