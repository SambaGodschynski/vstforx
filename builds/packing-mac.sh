#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForx-$1-mac
dst=mac/$vstr
folder=VSTForx/
mkdir -p $dst/$folder
cp -r ../src/PluginApps/vstforx.vst $dst/$folder
cp -r ../src/PluginApps/vstforxInstrument.vst $dst/$folder
cp license.txt $dst/$folder
cp readme-mac.txt $dst/$folder/readme.txt
cp ../Documents/manual/vstforx.EN/main.pdf $dst/$folder/manual.pdf
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
