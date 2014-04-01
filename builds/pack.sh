#!/bin/sh

function packVSTForx() {
    src=$1
    vstr=$2
    post=$3
    target=$4
    folder=$5
    ext=$6
    mkdir -p $target/$folder
    cp -r $src/vstforx$post.$ext $target/$folder
    cp -r $src/vstforxInstrument$post.$ext $target/$folder
    cp license.txt $target/$folder
    cp ../Documents/manual/vstforx.EN/main.pdf $target/$folder/manual.pdf
    cd $target
    zip -r $vstr.zip $folder
    rm -rf $folder
    cd -
}

function packRemote() {
    src=$1
    vstr=$2
    post=$3
    target=$4
    folder=$5
    ext=$6
    mkdir -p $target/$folder
    cp -r $src/remoteChannelSender$post.$ext $target/$folder
    cp license.txt $target/$folder
    cd $target
    zip -r $vstr.zip $folder
    rm -rf $folder
    cd -
}

################################################################################

if [ $# -ne 2 ]
then
  echo 'usage: $0 target[mac|win|win64] version'
  exit 1
fi

version=$2

if [ $1 = "mac" ]
then
    echo mac $version:
    packVSTForx ../src/PluginApps VSTForx-$version-mac '' mac VSTForx vst
    packVSTForx ../src/PluginApps VSTForxDEMO-$version-mac DEMO mac VSTForx vst
    packRemote ../src/PluginApps RemoteChannelSender-$version-mac '' mac VSTForx vst
    packRemote ../src/PluginApps RemoteChannelSenderDEMO-$version-mac DEMO mac VSTForx vst
    
elif [ $1 = "win" ] 
then
    echo win $version:
    packVSTForx ../src/PluginApps/Release VSTForx-$version-win '' win VSTForx dll
    packVSTForx ../src/PluginApps/Release VSTForxDEMO-$version-win DEMO win VSTForx dll
    packRemote ../src/PluginApps/Release RemoteChannelSender-$version-win '' win VSTForx dll
    packRemote ../src/PluginApps/Release RemoteChannelSenderDEMO-$version-win DEMO win VSTForx dll

elif [ $1 = "win64" ] 
then
    echo win64 $version:
    packVSTForx ../src/PluginApps/Release VSTForx-$version-win64 '' win VSTForx64 dll
    packVSTForx ../src/PluginApps/Release VSTForxDEMO-$version-win64 DEMO win VSTForx64 dll
    packRemote ../src/PluginApps/Release RemoteChannelSender-$version-win64 '' win VSTForx64 dll
    packRemote ../src/PluginApps/Release RemoteChannelSenderDEMO-$version-win64 DEMO win VSTForx64 dll
fi
