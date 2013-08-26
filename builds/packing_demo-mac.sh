#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForxDEMO-$1-mac
dst=mac/$vstr
folder=VSTForx/
mkdir -p $dst/$folder
cp -r ../src/vstforxDEMO.vst $dst/$folder
cp -r ../src/vstforxInstrumentDEMO.vst $dst/$folder
cp license.txt $dst/$folder
cp readme_demo-mac.txt $dst/$folder/readme.txt
cp ../Documents/manual/vstforx.EN/main.pdf $dst/$folder/manual.pdf
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
