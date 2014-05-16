function openClose(num)
  for x = 0, num, 1 do
    frx.openEditor()
    frx.closeEditor()
  end
end

frx.openPlugin()
openClose(3)

frx.closePlugin()
frx.openPlugin()
frx.openEditor()


frx.openPlugin()
frx.openPlugin()
frx.openEditor()
frx.openEditor()
frx.closeEditor()
frx.closeEditor()
frx.closePlugin()
frx.closePlugin()


-- first close plugin then editor
frx.closePlugin()
frx.closeEditor()







