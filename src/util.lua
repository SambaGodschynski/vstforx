function load(s)
    f=io.open(s,"r")
    s=f.read(f,"*a")
    frxDeserializePlugin(s)
    f.close(f)
end
