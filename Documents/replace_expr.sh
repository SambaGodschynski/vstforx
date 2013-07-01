#!/bin/sh

if [ ! -d $1 ]; then
        echo no directory given
	exit 1
fi

rep() {
	for x in $1/*.tex; do
		mv $x $x.bak
		sed 's/^[ ]*\\'$2'[ ]*{\(.*\)}/\\'$3'{\1}/' $x.bak > $x 
		rm $x.bak
	done
}

rep $1 section kap
rep $1 subsection ukap
rep $1 subsubsection uukap
