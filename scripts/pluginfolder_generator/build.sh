#!/bin/sh

#   \testVstFolder          <- no plugs
#   \testVstFolder\A        <- mda collection
#   \testVstFolder\B        <- mda collection + no plug files 
#   \testVstFolder\C'	    <- one plug that filename has special character
#   \testVstFolder\B\B1
#   \testVstFolder\B\B1\B1_1 <- textfile
#   \testVstFolder\B\B2	     <- mda collection but overdrive == PLUGIN_LOACTION_1 => renamed
#   \testVstFolder\B\B3      <- no plug files




export PATH=$PATH:$(pwd)/../../src/PlugBox/src

function usage() 
{
	echo $1 --plattform --arch
}


#passing arguments
while [ "$1" != "" ]; do
    case $1 in
        -p | --plattform )      shift
                                platt=$1
                                ;;
        -a | --arch )           shift
                                arch=$1
                                ;;
         * )                    usage
                                exit 1
    esac
    shift
done

if [ -z $platt ] 
then
	echo no plattform specifcied
	exit 1
fi

dst="../../src/testVstFolder"

plugBox.py testplugs.xml sync . --plattform $platt

if [ $platt="mac" ] #mda mac has five plugs more than win
then
    rm -r "testVstFolder/mda/mda Looplex.vst"
    rm -r "testVstFolder/mda/mda RezFilter.vst"
    rm -r "testVstFolder/mda/mda Degrade.vst"
    rm -r "testVstFolder/mda/mda Image.vst"
    rm -r "testVstFolder/mda/mda Shepard.vst"
fi

if [ -d $dst ]; then
  rm -r $dst
fi

mv  testVstFolder $dst
mv  $dst/mda $dst/A
cp -r $dst/A $dst/B
cp -r $dst/B "$dst/C'"
cp -r $dst/A $dst/B/B2


#mda BeatBox.vst
