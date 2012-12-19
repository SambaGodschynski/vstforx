-- close, open editor and check
-- whether derserialization succeed
function doOpenCloseSequence()
  old = frxGetComponents()
  frxCloseEditor()
  frxOpenEditor()
  new = frxGetComponents()
  assert(#old == #new, "deserialization failed")
end

-- close, open editor/plug and check
-- whether derserialization succeed
function doOpenClosePlugSequence()
  old = frxGetComponents()
  frxClosePlugin()
  frxCloseEditor()
  frxOpenPlugin()
  frxOpenEditor()
  new = frxGetComponents()
  assert(#old == (#new), "deserialization failed")
end


checkZero = frxGetComponents()
assert( #checkZero == 0)

frxOpenPlugin()
frxOpenEditor()
components = frxGetComponents()
assert(#components == 2)

p = frxGetProcessors()  -- insert all possible processors
for i, x in pairs(p) do
    frxAddProcessor(x)
end

doOpenCloseSequence()
doOpenCloseSequence()
doOpenClosePlugSequence()
doOpenClosePlugSequence()

--serialize plug, close plugin
stream = frxSerializePlugin()
beforClosePlug = frxGetComponents()
frxCloseEditor()
frxClosePlugin()

frxOpenPlugin()
frxOpenEditor()

assert(#frxGetComponents() == #beforClosePlug, "content gone after close plugin")

frxGetComponents()






