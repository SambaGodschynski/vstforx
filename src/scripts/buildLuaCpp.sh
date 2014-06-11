echo "LuaClassBuilder.py --otype=header $1 > $2.hpp"
echo "LuaClassBuilder.py --otype=impl $1 > $2.cpp"
LuaClassBuilder.py --otype=header $1 > $2.hpp
LuaClassBuilder.py --otype=impl $1 > $2.cpp
