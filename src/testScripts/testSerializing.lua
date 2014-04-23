-- ### BEGIN FUNCTIONS

function clearView()
   viewHelper.removeAll()
   num = getNumElements()
   -- entry + exit = 2
   assert( num == 2, "clear view failed:"..tostring(num))
end

OP = frx.openPlugin
OE = frx.openEditor
CP = frx.closePlugin
CE = frx.closeEditor
CV = clearView
function doSequence(seq)
   for i, x in pairs(seq) do
      x()
      frx.wait(100)
   end
end

--brute connecting of all components on view
function connectAllComponents()
   a = viewHelper.getConnectables()
   b = a
   for i, x in pairs(a) do
      for j, y in pairs(b) do 
	 if not (x == y) then
	    if frx.view:connect(x,y) == true then
	       --frx.wait(600)
	    end	
	 end		
      end	
   end
end

-- assumes that the editor is isOpen
-- to get the elements
function getNumElements()
   assert(frx.isEditorOpen() == true, "assert(frx.isEditorOpen())")
   return #frx.view:getObjects()
end

-- assumes that the last sequence is open editor
-- to get the elements
function doSequenceAssertElements(seq, numElements)
   for i, x in pairs(seq) do
      x()
   end
   ist=getNumElements()
   assert(ist == numElements, 
	  "deserialization failed.((soll)"..tostring(numElements)..", (ist)"..tostring(ist)..")")
end
-- ### END FUNCTIONS


--for ccc = 1, 100,1 do

doSequence({OP, OE})


p = viewHelper.getProcessorTypeNames()  -- insert all available processors
for i, x in pairs(p) do
   new = frx.view:add(x)
end

connectAllComponents()

numElements = getNumElements()

-- do some editor, plugin open/close sequences
doSequenceAssertElements({CE, OE}, numElements)
--[[doSequenceAssertElements({CE, OE}, numElements)
doSequenceAssertElements({CP, CE, OP, OE}, numElements)
doSequenceAssertElements({CP, CE, OP, OE}, numElements)

doSequenceAssertElements({CE, CP, OE, OP}, numElements)

-- editor is open, serialize / editor is closed, deserialize
stream = frx.serializePlugin()
doSequence({CV, CE, CP})
frx.deserializePlugin(stream)
doSequenceAssertElements({OP, OE}, numElements)


-- editor is open, serialize / editor is open, deserialize
stream = frx.serializePlugin()
doSequence({CV})
frx.deserializePlugin(stream)
assert(getNumElements() == numElements)

-- editor is closed, serialize / editor is closed, deserialize
doSequence({CE})
stream = frx.serializePlugin()
frx.deserializePlugin(stream)
doSequenceAssertElements({OE}, numElements)


-- editor is closed, serialize / editor is open, deserialize
doSequence({CE})
stream = frx.serializePlugin()
doSequence({OE, CV})
frx.deserializePlugin(stream)
assert(getNumElements() == numElements)


-- editor is open, serialize / !no deserialize
stream = frx.serializePlugin()
doSequenceAssertElements({CE, CP, OE, OP}, numElements)
doSequence({CV})
]]--
--end
