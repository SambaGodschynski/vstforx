frx.openPlugin()
frx.openEditor()

pr = frx.addProcessor("internal.InputStep")
ins = frx.getProcessorInputs(pr)
outs = frx.getProcessorOutputs(pr)
entry, exit = frx.getEntryExit()
frx.connectComponents(entry, ins[1])
frx.connectComponents(exit, outs[1])


s=frx.serializePlugin()

frx.closeEditor()
frx.deserializePlugin(s)
frx.openEditor()
