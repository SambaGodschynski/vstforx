frx.openPlugin()
frx.openEditor()

ad = frx.view:add("internal.ADSRTrigger")
st = frx.view:add("internal.InputStep")
sti = st:getInputs()
sto = st:getOutputs()
ai = ad:getInputs()
entry, exit = frx.view:getEntry(), frx.view:getExit()


frx.view:connect(exit, sto[1])
frx.view:connect(ai[1], sto[1])

for i = 1,1000,1 do
	--print("try #"..tostring(i))
	--impossible connection:
	frx.view:connect(sti[1], sto[1])
end
