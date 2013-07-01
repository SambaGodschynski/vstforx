frxOpenPlugin()
frxOpenEditor()

p = frxGetProcessorTypes() -- insert all available processors
for i, x in pairs(p) do
	frxAddProcessor(x)
end

-- brute force connection
a = frxGetViewNodes()
b = a
for i, x in pairs(a) do
	for j, y in pairs(b) do 
		if not (x == y) then
			if ( frxConnectComponents(x,y) == frxTrue() ) then 
				--print(frxGetComponentName(x), frxGetComponentName(y))
				--frxWait(600)
			end
		end		
	end	
end
