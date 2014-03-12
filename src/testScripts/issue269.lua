frx.openPlugin()
frx.openEditor()

ad = frx.addProcessor("internal.ADSRTrigger")
st = frx.addProcessor("internal.InputStep")
sti = frx.getProcessorInputs(st)
sto = frx.getProcessorOutputs(st)
ai = frx.getProcessorInputs(ad)
entry, exit = frx.getEntryExit()


frx.connectComponents(exit, sto[1])
frx.connectComponents(ai[1], sto[1])

for i = 1,1000,1 do
	--print("try #"..tostring(i))
	--impossible connection:
	frx.connectComponents(sti[1], sto[1])
end
