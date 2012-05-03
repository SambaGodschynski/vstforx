--setup
gpParameterSetup = {}
_i = {}

function lcInit()
         for i=1, 9999, 1 do
             _ENV._i[i] = 0
         end
end

function lcProcess(numSamples)
         l, r = frxGetFramesFromInput(1)
         r, _i = frxFFT(r, i, numSamples)
         _i, r = frxFFT(_i, r, numSamples)
         frxSetFramesToOutput(1, r, r)
end