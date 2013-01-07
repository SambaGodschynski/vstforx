function load(s)
    f=io.open(s,"r")
    s=f.read(f,"*a")
    frxDeserializePlugin(s)
    f.close(f)
end

function save(fname)
    f=io.open(fname,"w")
    s=frxSerializePlugin()
    f.write(f,s)
    f.close(f)
end
