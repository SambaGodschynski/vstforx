--setup
gpParameterSetup = {}
_i = {}

function lcInit()
         for i=1, 9999, 1 do
             if i==10 then
                _ENV._i[i] = 1
             end
             _ENV._i[i] = 0
         end
end

function lcProcess(numSamples)
         l, r = frxGetFramesFromInput(1)
         _i, r = frxFFT(r, _i, numSamples)
         --_i, r = frxFFT(_i, r, numSamples)
         frxSetFramesToOutput(1, r, r)
end