#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=VSTForx-$1-mac
dst=mac/$vstr
folder=VSTForx.beta/
mkdir -p $dst/$folder
prj=../VSTForx-mac/vstforx-ppc/build/Release/
cp -r -f $prj/VSTForx.vst $dst/$folder/VSTForx.vst
cp -r -f $prj/iVSTForx.vst $dst/$folder/iVSTForx.vst
cp license.txt $dst/$folder
cp readme-mac.txt $dst/$folder/readme.txt
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
