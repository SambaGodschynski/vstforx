frx.openPlugin()
frx.openEditor()

p = viewHelper.getProcessorTypeNames() -- insert all available processors
for i, x in pairs(p) do
   frx.view:add(x)
end

--brute force connection
a = viewHelper.getConnectables()
b = a
for i, x in pairs(a) do
   for j, y in pairs(b) do 
      if not (x == y) then
	 if ( frx.view:connect(x,y) == true ) then
	    --print(frx.getComponentName(x), frx.getComponentName(y))
	    --frx.wait(600)
	 end
      end		
   end	
end
