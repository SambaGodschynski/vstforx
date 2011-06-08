from re import *

f = open ( "testplug02.rc", "r" )
_str = f.read()
f.close()
l = findall ('IDB_PNG[0-9]*', _str )
y = 10001
for x in l:
  print '#define ' + x + ' ' + str(y)
  y+=1
