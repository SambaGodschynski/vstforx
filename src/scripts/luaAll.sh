for x in $(echo *.luaCpp)
do
  in=$x
  out=$(echo $x | sed 's/.luaCpp//')Base
  echo build $in $out
  LuaClassBuilder.py $in --otype=header > $out.hpp
  LuaClassBuilder.py $in --otype=impl > $out.cpp
done
