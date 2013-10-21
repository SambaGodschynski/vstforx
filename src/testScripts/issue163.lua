function assertGraphDelay(d)
	 assert( frxGetGraphDelay() == d, 
	 	 "graph delay. expected: " .. d .. " actual: " .. frxGetGraphDelay() ) 
end




frxOpenPlugin()
frxOpenEditor()

assertGraphDelay(0)

p = frxAddProcessor("FrxDCTester")
assert(#p>0, "adding FrxDCTester failed. Is FRX_FEATURE_DC_TESTER enabled?");
delay = frxGetComponentParameter(p)[1]

entry, exit = frxGetEntryExit()

-- change delay BEFORE DCTester is hooked in
frxSetParameterValue(delay, 1)
frxConnectComponents( frxGetProcessorInputs(p)[1], entry )
frxConnectComponents( frxGetProcessorOutputs(p)[1], exit )
assertGraphDelay(32768)

-- change delay AFTER DCTester is hooked in
frxSetParameterValue(delay, 0)
assertGraphDelay(0)

frxSetParameterValue(delay, 1)
assertGraphDelay(32768)
save = frxSerializePlugin()
frxClearView()

frxClosePlugin()
frxCloseEditor()

frxOpenPlugin()
frxOpenEditor()
assertGraphDelay(0)
frxDeserializePlugin(save)
assertGraphDelay(32768)