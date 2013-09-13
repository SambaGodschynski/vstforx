#!/bin/sh

function samba_loop() 
{
	for (( c=1; c<=$2; c++ ))
	do
		echo "...........................................................................$c"
		$1
		if [ $? -ne 0 ]
		then
			echo !abort
			break
		fi
	done
}

export -f samba_loop
echo "samba_loop() exported"
