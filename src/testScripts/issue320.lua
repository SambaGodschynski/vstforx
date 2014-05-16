frx.openPlugin()
frx.openEditor()

sw = frx.view:add("internal.InputSwitch")
swi = sw:getInputs()
swo = sw:getOutputs()
entry, exit = frx.view:getEntry(), frx.view:getExit()


frx.view:connect(exit, swo[1])

data = frx.serializePlugin()

frx.wait(500)
viewHelper.removeAll()
frx.wait(500)

frx.deserializePlugin(data)

frx.wait(500)

frx.closeEditor()
frx.closePlugin()

