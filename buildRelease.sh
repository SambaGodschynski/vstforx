#!/bin/sh
set -u

CLIBS=/Users/samba/clibs
SAMBA_USELOG=0
NEWEST=true
SAMBAG_BRANCH=master
VSTFORX_BRANCH=DISCO
SSH_USER=samba
SSH_SERVER=johanness-mini.fritz.box
SAMBAG_REMOTE_LOC=$SSH_USER@$SSH_SERVER:workspace/sambag
VSTFORX_REMOTE_LOC=$SSH_USER@$SSH_SERVER:workspace/vstforx
ROOT=$(pwd)
TARGET=$SSH_USER@$SSH_SERVER:workspace/VSTForxBuilds

function __nameWorkaround() {
  if [ -d $2 ]
  then
    rm -r $2 
  fi
  mv $1 $2
  echo "BNDL????" > $2/Contents/PkgInfo
}

cd ../sambag

if [ "$NEWEST" = "true" ]
then
    if [ -f Makefile ]
    then
	make clean
    fi
  git stash
  git checkout $SAMBAG_BRANCH
  git pull $SAMBAG_REMOTE_LOC $SAMBAG_BRANCH
  if [ -f CMakeCache.txt ]
  then
      rm CMakeCache.txt
  fi
  cd scripts
  python cmakeWalker.py
  cd ..
fi
sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
make


cd $ROOT

if [ "$NEWEST" = "true" ]
then
    if [ -f Makefile ]
    then
	make clean
    fi
  git stash
  git checkout $VSTFORX_BRANCH
  git pull $VSTFORX_REMOTE_LOC $VSTFORX_BRANCH
  if [ -f CMakeCache.txt ]
  then
      rm CMakeCache.txt
  fi
  cd scripts
  python cmakeWalker.py
  cd ..
fi
sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
make

cd src/PluginApps
__nameWorkaround vstforx.app vstforx.vst
__nameWorkaround vstforxInstrument.app vstforxInstrument.vst
__nameWorkaround vstforxInstrument_2Out.app vstforxInstrument_2Out.vst
__nameWorkaround vstforxInstrument_4Out.app vstforxInstrument_4Out.vst
__nameWorkaround vstforxInstrument_8Out.app vstforxInstrument_8Out.vst
__nameWorkaround vstforxDEMO.app vstforxDEMO.vst
__nameWorkaround vstforxInstrumentDEMO.app vstforxInstrumentDEMO.vst
__nameWorkaround vstforxInstrumentDEMO_2Out.app vstforxInstrumentDEMO_2Out.vst
__nameWorkaround vstforxInstrumentDEMO_4Out.app vstforxInstrumentDEMO_4Out.vst
__nameWorkaround vstforxInstrumentDEMO_8Out.app vstforxInstrumentDEMO_8Out.vst


#packing VSTForx
cd $ROOT/builds

rm -rf mac/*
sh packLastBuild.sh mac

scp mac/* $TARGET

cd $ROOT

