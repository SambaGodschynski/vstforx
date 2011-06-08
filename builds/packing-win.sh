#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForx-$1-win
dst=win/$vstr
folder=VSTForx.beta/
mkdir -p $dst/$folder
cp ../VSTForx/Release/VSTForx.dll $dst/$folder
cp ../VSTForx/iRelease/iVSTForx.dll $dst/$folder
cp license.txt $dst/$folder
cp readme-win.txt $dst/$folder/readme.txt
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
