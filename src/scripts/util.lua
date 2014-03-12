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
    	frx.closeEditor()
        frx.wait(200)
	frx.openEditor()
        frx.wait(200)
    end
end

function findByName(name)
  c=frx.getViewComponents()
  for k, v in pairs(c) do
      if name == frx.getComponentName(v) then
      	 return v
      end
  end
  return ""
end
