#!/bin/sh

#passing arguments
sambag=""
function parse() 
{
    while [ "$1" != "" ]; do
	case $1 in
            -i | --in )             
		shift
		if  [ -z $sambag ]
		then
		    sambag=$1/sambag
		fi
		;;
	    -s | --sambag )           
		shift
		sambag=$1
		;;
	esac
	shift
    done
}
parse $*
sh $sambag/build.sh "$@"



