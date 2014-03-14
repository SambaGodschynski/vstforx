require "scripts/util"

function assertGraphDelay(d)
	frx.wait(1000) -- graph update happens async 
	assert( frx.getGraphDelay() == d, 
	 	 "graph delay. expected: " .. d .. " actual: " .. frx.getGraphDelay() ) 
end

frx.openPlugin()
frx.openEditor()

assertGraphDelay(0)

p = frx.addProcessor("internal-private.DCTester")
assert(#p>0, "adding FrxDCTester failed. Is FRX_FEATURE_DC_TESTER enabled?");
delay = frx.getComponentParameter(p)[1]

entry, exit = frx.getEntryExit()

-- change delay BEFORE DCTester is hooked in
frx.setParameterValue(delay, 1)
frx.connectComponents( frx.getProcessorInputs(p)[1], entry )
frx.connectComponents( frx.getProcessorOutputs(p)[1], exit )
assertGraphDelay(32768)

-- change delay AFTER DCTester is hooked in
frx.setParameterValue(delay, 0)
assertGraphDelay(0)

frx.setParameterValue(delay, 1)
assertGraphDelay(32768)
save = frx.serializePlugin()
frx.clearView()

frx.closePlugin()
frx.closeEditor()
-- !! object pointer are invalid now

frx.openPlugin()
frx.openEditor()
assertGraphDelay(0)
frx.deserializePlugin(save)
assertGraphDelay(32768)

p = findByName("DC Tester")
assert(#p>0)
delay = frx.getComponentParameter(p)[1]
frx.setParameterValue(delay, 0)
assertGraphDelay(0)
