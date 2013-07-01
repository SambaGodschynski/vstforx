frxOpenPlugin()
frxOpenEditor()

function addSwitch(name)
	sw = frxAddProcessor(name)
	swo = frxGetProcessorOutputs(sw)
	entry, exit = frxGetEntryExit()
	frxConnectComponents(exit, swo[1])
	return sw
end

isw = addSwitch("FrxInSwitch")
ist = addSwitch("FrxInStep")
osw = addSwitch("FrxOutSwitch")
ost = addSwitch("FrxOutStep")

for i = 1,10,1 do 
	frxAddProcessorInput(isw)
end

for i = 1,10,1 do 
	frxAddProcessorInput(ist)
end

for i = 1,10,1 do 
	frxAddProcessorOutput(osw)
end
for i = 1,10,1 do 
	frxAddProcessorOutput(ost)
end

frxWait(100)

frxClosePlugin()
frxCloseEditor()
