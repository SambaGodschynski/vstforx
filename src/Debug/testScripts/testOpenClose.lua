function openClose(num)
  for x = 0, num, 1 do
    frxOpenEditor()
    frxCloseEditor()
  end
end

frxOpenPlugin()
openClose(3)

frxClosePlugin()
frxOpenPlugin()
frxOpenEditor()

-- first close plugin then editor
--frxClosePlugin()
--frxCloseEditor()







