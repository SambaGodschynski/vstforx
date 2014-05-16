require "scripts/util"

function assertGraphDelay(d)
	frx.wait(1000) -- graph update happens async 
	assert( frx.getGraphDelay() == d, 
	 	 "graph delay. expected: " .. d .. " actual: " .. frx.getGraphDelay() ) 
end

frx.openPlugin()
frx.openEditor()

assertGraphDelay(0)

p = frx.view:add("internal-private.DCTester")
delay = p:getParameters()[1]

entry, exit = frx.view:getEntry(), frx.view:getExit()

-- change delay BEFORE DCTester is hooked in
delay:setValue(1)
frx.view:connect( p:getInputs()[1], entry )
frx.view:connect( p:getOutputs()[1], exit )
assertGraphDelay(32768)

-- change delay AFTER DCTester is hooked in
delay:setValue(0)
assertGraphDelay(0)

delay:setValue(1)
assertGraphDelay(32768)
save = frx.serializePlugin()
viewHelper.removeAll()

frx.closePlugin()
frx.closeEditor()
-- !! object pointer are invalid now

frx.openPlugin()
frx.openEditor()
assertGraphDelay(0)
frx.deserializePlugin(save)
assertGraphDelay(32768)

p = findByName("DC Tester")
delay = p:getParameters()[1]
delay:setValue(0)
assertGraphDelay(0)
