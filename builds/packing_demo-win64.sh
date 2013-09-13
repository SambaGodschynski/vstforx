#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForxDEMO-$1-win64
dst=win64/$vstr
folder="VSTForx (x86_64)/"
mkdir -p $dst/"$folder"
cp ../src/PluginApps/Release/vstforxDEMO.dll "$dst/$folder/vstforxDEMO (x86_64).dll"
cp ../src/PluginApps/Release/vstforxInstrumentDEMO.dll "$dst/$folder/vstforxInstrumentDEMO (x86_64).dll"
cp license.txt $dst/"$folder"
cp readme_demo-win.txt $dst/"$folder"/readme.txt
cp ../Documents/manual/vstforx.EN/main.pdf $dst/"$folder"/manual.pdf
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
