function load(s)
    f=io.open(s,"r")
    s=f.read(f,"*a")
    frx.deserializePlugin(s)
    f.close(f)
end

function save(fname)
    f=io.open(fname,"w")
    s=frx.serializePlugin()
    f.write(f,s)
    f.close(f)
end

function oc(x)
    for i = 0, x, 1 do
	print("closing")
    	frx.closeEditor()
        frx.wait(1000)
	print("opening")
	frx.openEditor()
        frx.wait(1000)
    end
end

function findByName(name)
    return frx.view:getByName(name)
end
