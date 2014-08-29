frx.openPlugin()
frx.openEditor()

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

load("testScripts/legacy-1.0.42")
frx.wait(2000)
save("tmp")
frx.wait(2000)
load("tmp")
frx.wait(2000)


load("testScripts/legacy-1.0.54")


frx.wait(2000)

frx.closeEditor()
frx.closePlugin()
