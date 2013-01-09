frxOpenPlugin()
frxOpenEditor()

pr = frxAddProcessor("FrxInStep")
ins = frxGetProcessorInputs(pr)
outs = frxGetProcessorOutputs(pr)
entry, exit = frxGetEntryExit()
frxConnectComponents(entry, ins[1])
frxConnectComponents(exit, outs[1])

s=frxSerializePlugin()
frxCloseEditor()
frxDeserializePlugin(s)
frxOpenEditor()
