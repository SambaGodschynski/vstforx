function luaCppUpload() 
{
  LuaClassBuilder.py $1 --otype=doc > $3
  scp $3 sambag@sambag.carina.uberspace.de:html/forx/FrxLuaDoc/l1/$2/$3
  rm $3
}
