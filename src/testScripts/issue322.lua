frx.openPlugin()
frx.openEditor()

function addSwitch(name)
	sw = frx.addProcessor(name)
	swo = frx.getProcessorOutputs(sw)
	entry, exit = frx.getEntryExit()
	frx.connectComponents(exit, swo[1])
	return sw
end

isw = addSwitch("internal.InputSwitch")
ist = addSwitch("internal.InputStep")
osw = addSwitch("internal.OutputSwitch")
ost = addSwitch("internal.OutputStep")

for i = 1,10,1 do 
	frx.addProcessorInput(isw)
end

for i = 1,10,1 do 
	frx.addProcessorInput(ist)
end

for i = 1,10,1 do 
	frx.addProcessorOutput(osw)
end
for i = 1,10,1 do 
	frx.addProcessorOutput(ost)
end

frx.wait(100)

frx.closePlugin()
frx.closeEditor()
