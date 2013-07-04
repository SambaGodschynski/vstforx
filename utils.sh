#!/bin/sh

function samba_loop() 
{
	for x in $(seq $2)
	do
		echo run $x:
		echo "<<<<<<<<<<>>>>>>>>>>"
		$1
	done
}

export -f samba_loop
echo samba_loop() exported
