#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForxDEMO-$1-win
dst=win/$vstr
folder=VSTForx/
mkdir -p $dst/$folder
cp ../src/Release/vstforxDEMO.dll $dst/$folder
cp ../src/Release/vstforxInstrumentDEMO.dll $dst/$folder
cp license.txt $dst/$folder
cp readme_demo-win.txt $dst/$folder/readme.txt
cp ../Documents/manual/vstforx.EN/main.pdf $dst/$folder/manual.pdf
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
