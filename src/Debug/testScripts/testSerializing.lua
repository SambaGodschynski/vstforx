function wait()
  frxWait(500) --in hardcore mode it should work without waiting
end

frxOpenPlugin()
wait()
frxOpenEditor()
wait()

p = frxGetProcessors()
for i, x in pairs(p) do
    print(x)
    frxAddProcessor(x)
end


wait()
frxCloseEditor()
wait()

frxOpenEditor()
wait()

frxSetEditorExitOnClose(1)
frxCloseEditor()

frxClosePlugin()




