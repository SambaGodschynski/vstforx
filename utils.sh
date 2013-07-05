#!/bin/sh

function samba_loop() 
{
	for (( c=1; c<=$2; c++ ))
	do
		echo "...........................................................................$c"
		$1
	done
}

export -f samba_loop
echo "samba_loop() exported"
