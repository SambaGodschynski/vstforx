#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=RemoteChannelSender-$1-win
dst=win/$vstr
folder=VSTForx/
mkdir -p $dst/$folder
cp ../src/PluginApps/Release/remoteChannelSender.dll $dst/$folder
cp license.txt $dst/$folder
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
