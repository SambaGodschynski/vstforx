frx.openPlugin()
frx.openEditor()

function addSwitch(name)
	sw = frx.view:add(name)
	swo = sw:getOutputs()
	entry, exit = frx.view:getEntry(), frx.view:getExit()
	frx.view:connect(exit, swo[1])
	return sw
end

isw = addSwitch("internal.InputSwitch")
ist = addSwitch("internal.InputStep")
osw = addSwitch("internal.OutputSwitch")
ost = addSwitch("internal.OutputStep")

for i = 1,10,1 do 
	isw:addInput()
end

for i = 1,10,1 do 
	ist:addInput()
end

for i = 1,10,1 do 
	osw:addOutput()
end
for i = 1,10,1 do 
	ost:addOutput()
end

frx.wait(100)

frx.closePlugin()
frx.closeEditor()
