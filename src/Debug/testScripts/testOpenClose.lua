function wait()
  frxWait(500) --in hardcore mode it should work without waiting
end

function openClose(num)
  for x = 0, num, 1 do
    frxOpenEditor()
    wait()
    frxCloseEditor()
    wait()
  end
end

frxOpenPlugin()
wait()
openClose(3)

frxClosePlugin()
frxOpenPlugin()
print("now an exception:")
frxOpenEditor()
wait()
frxSetEditorExitOnClose(1)
frxCloseEditor()
frxClosePlugin()





