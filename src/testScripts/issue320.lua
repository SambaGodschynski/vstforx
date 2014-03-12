frx.openPlugin()
frx.openEditor()

sw = frx.addProcessor("internal.InputSwitch")
swi = frx.getProcessorInputs(sw)
swo = frx.getProcessorOutputs(sw)
entry, exit = frx.getEntryExit()


frx.connectComponents(exit, swo[1])

data = frx.serializePlugin()

frx.wait(500)
frx.clearView()
frx.wait(500)

frx.deserializePlugin(data)

frx.wait(500)

frx.closeEditor()
frx.closePlugin()

