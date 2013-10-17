#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForx-CM-$1-win64
dst=win64/$vstr
folder="VSTForx-CM (x86_64)/"
mkdir -p $dst/"$folder"
cp ../src/PluginApps/Release/VSTForx-CM.dll "$dst/$folder/VSTForx-CM (x86_64).dll"
cp ../src/PluginApps/Release/VSTForxInstrument-CM.dll "$dst/$folder/VSTForxInstrument-CM (x86_64).dll"
cp license.txt $dst/"$folder"
cp readme-win.txt $dst/"$folder"/readme.txt
cp ../Documents/manual/vstforx.EN/main.pdf $dst/"$folder"/manual.pdf
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
