-- ### BEGIN FUNCTIONS

function clearView()
    frxClearView()
    num = getNumElements()
	-- entry + exit + connection = 3
    assert( num == 3, "clear view failed:"..tostring(num))
end

OP = frxOpenPlugin
OE = frxOpenEditor
CP = frxClosePlugin
CE = frxCloseEditor
CV = clearView
function doSequence(seq)
  for i, x in pairs(seq) do
    x()
  end
end

--brute connecting of all components on view
function connectAllComponents()
	a = frxGetViewNodes()
	b = a
	for i, x in pairs(a) do
		for j, y in pairs(b) do 
			if not (x == y) then
				--print(frxGetComponentName(x)..x, frxGetComponentName(y)..y)
				frxConnectComponents(x,y)		
			end		
		end	
	end
end

-- assumes that the editor is isOpen
-- to get the elements
function getNumElements()
    assert(frxIsEditorOpen() == frxTrue(), "assert(frxIsEditorOpen())")
    components = frxGetViewComponents()
    return #components
end

-- assumes that the last sequence is open editor
-- to get the elements
function doSequenceAssertElements(seq, numElements)
  for i, x in pairs(seq) do
    x()
  end
  assert(getNumElements() == numElements, "deserialization failed.("..tostring(getNumElements())..")")
end
-- ### END FUNCTIONS

--frxVerbose(frxTrue())

doSequence({OP, OE})


assert(getNumElements() == 2, tostring(#components))

p = frxGetProcessors()  -- insert all possible processors
for i, x in pairs(p) do
    new = frxAddProcessor(x)
end

connectAllComponents()

c = frxGetViewComponents()
for i, x in pairs(c) do	
	pars = frxGetComponentParameter(x)
	for j, y in pairs(pars) do
		--frxAddComponentParameter(x ,y)
	end
end



numElements = getNumElements()

-- do some editor, plugin open/close sequences
doSequenceAssertElements({CE, OE}, numElements)
doSequenceAssertElements({CE, OE}, numElements)
doSequenceAssertElements({CP, CE, OP, OE}, numElements)
doSequenceAssertElements({CP, CE, OP, OE}, numElements)

doSequenceAssertElements({CE, CP, OE, OP}, numElements)

-- editor is open, serialize / editor is closed, deserialize
stream = frxSerializePlugin()
doSequence({CV, CE, CP})
frxDeserializePlugin(stream)
doSequenceAssertElements({OP, OE}, numElements)


-- editor is open, serialize / editor is open, deserialize
stream = frxSerializePlugin()
doSequence({CV})
frxDeserializePlugin(stream)
assert(getNumElements() == numElements)

-- editor is closed, serialize / editor is closed, deserialize
doSequence({CE})
stream = frxSerializePlugin()
frxDeserializePlugin(stream)
doSequenceAssertElements({OE}, numElements)


-- editor is closed, serialize / editor is open, deserialize
doSequence({CE})
stream = frxSerializePlugin()
doSequence({OE, CV})
frxDeserializePlugin(stream)
assert(getNumElements() == numElements)


-- editor is open, serialize / !no deserialize
stream = frxSerializePlugin()
doSequenceAssertElements({CE, CP, OE, OP}, numElements)
doSequence({CV})


