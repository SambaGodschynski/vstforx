frx.openPlugin()
frx.openEditor()

pr = frx.view:add("internal.InputStep")
ins = pr:getInputs()
outs = pr:getOutputs()
entry, exit = frx.view:getEntry(), frx.view:getExit()
frx.view:connect(entry, ins[1])
frx.view:connect(exit, outs[1])


s=frx.serializePlugin()

frx.closeEditor()
frx.deserializePlugin(s)
frx.openEditor()
