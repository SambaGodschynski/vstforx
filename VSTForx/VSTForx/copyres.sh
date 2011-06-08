#!/bin/sh

#extrahiert ressourcen und kopiert nach $1

y=10000 
for x in $(python extractResourceLocation.py)
do 
  y=$(expr $y + 1)
  cp -v $(pwd)/$x $(pwd)/$1/bmp$y.png
done
