#!/bin/sh

function __nameWorkaround() {
  if [ -d $2 ]
  then
    rm -r $2 
  fi
  mv $1 $2
  echo "BNDL????" > $2/Contents/PkgInfo
}


function build() {
  ORG=$(pwd)
  cd ~/workspace/sambag
  sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
  make
  cd ~/workspace/vstforx
  sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
  make
  cd src/PluginApps
  __nameWorkaround vstforx.app vstforx.vst
  __nameWorkaround vstforxInstrument.app vstforxInstrument.vst
  __nameWorkaround vstforxInstrument_O2.app vstforxInstrument_O2.vst
  __nameWorkaround vstforxInstrument_O4.app vstforxInstrument_O4.vst
  __nameWorkaround vstforxInstrument_O8.app vstforxInstrument_O8.vst
  __nameWorkaround VSTForx-CM.app VSTForx-CM.vst
  __nameWorkaround VSTForxInstrument-CM.app VSTForxInstrument-CM.vst
  __nameWorkaround vstforxDEMO.app vstforxDEMO.vst
  __nameWorkaround vstforxInstrumentDEMO.app vstforxInstrumentDEMO.vst
  cd $ORG
}
export -f __nameWorkaround
export -f build
echo build command exported.
