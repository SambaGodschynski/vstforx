frxOpenPlugin()
frxOpenEditor()

ad = frxAddProcessor("FrxADSR")
st = frxAddProcessor("FrxInStep")
sti = frxGetProcessorInputs(st)
sto = frxGetProcessorOutputs(st)
ai = frxGetProcessorInputs(ad)
entry, exit = frxGetEntryExit()


frxConnectComponents(exit, sto[1])
frxConnectComponents(ai[1], sto[1])

for i = 1,1000,1 do
	--print("try #"..tostring(i))
	--impossible connection:
	frxConnectComponents(sti[1], sto[1])
end
