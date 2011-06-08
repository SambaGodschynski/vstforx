from re import *

f = open ( "testplug02.rc", "r" )
str = f.read()
f.close()
l = findall ('IDB_PNG[0-9]*?.*?"(.*?)"', str )
for x in l:
  x = x.replace ('\\\\', '/')
  print x
