#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=RemoteChannelSender_DEMO-$1-win64
dst=win64/$vstr
folder="VSTForx (x86_64)/"
mkdir -p $dst/"$folder"
cp ../src/PluginApps/Release/remoteChannelSender_DEMO.dll "$dst/$folder/remoteChannelSender_DEMO (x86_64).dll"
cp license.txt $dst/$folder
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
