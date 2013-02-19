--TODO: add frxAddProcessorI/O to impl. these test
--[[frxOpenPlugin()
frxOpenEditor()

sw = frxAddProcessor("FrxInSwitch")
swi = frxGetProcessorInputs(sw)
swo = frxGetProcessorOutputs(sw)
entry, exit = frxGetEntryExit()


frxConnectComponents(exit, swo[1])

data = frxSerializePlugin()

frxWait(500)
frxClearView()
frxWait(500)

frxDeserializePlugin(data)

frxWait(500)

frxCloseEditor()
frxClosePlugin()]]

