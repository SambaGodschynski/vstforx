--setup
gpConfig = {
   type="frx_lua_plugin", 
   name="Volume", 
   author="Samba Godschynski",
   license="GPL",
   numInputs=2, 
   numOutputs=2
}
gpParameterSetup = {volume=1.0}

function lcProcess(numSamples)
   r = frxGetInput(1)
   l = frxGetInput(2)
   for i=1, numSamples, 1 do
      r[i] = r[i] * gpParameterSetup["volume"]
      l[i] = l[i] * gpParameterSetup["volume"]
   end 
   frxToOutput(1, r)
   frxToOutput(2, l)
end
