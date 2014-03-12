frx.openPlugin()
frx.openEditor()

p = frx.getProcessorTypes() -- insert all available processors
for i, x in pairs(p) do
	frx.addProcessor(x)
end

-- brute force connection
a = frx.getViewNodes()
b = a
for i, x in pairs(a) do
	for j, y in pairs(b) do 
		if not (x == y) then
			if ( frx.connectComponents(x,y) == frx.true() ) then
				--print(frx.getComponentName(x), frx.getComponentName(y))
				--frx.wait(600)
			end
		end		
	end	
end
