#!/bin/bash

#var USAGE = ""

echo "Usage: ./lab1.sh path/to/work <name pattern> <count of 1st level dir-s> <count of 2st level dir-s> <count of files>"

cd $1 #/Eltex
mkdir DIRS
cd DIRS

for ((i = 0; i < $3; i++))
do
	mkdir $2$i 
	cd $2$i
	for((j = 0; j < $4; j++))
	do
		mkdir $2$i.$j
		cd $2$i.$j
		for((k = 0; k < $5; k++))
		do
			touch file$i.$j.$k 
		done
		cd ../ 
	done
	cd ../
done

pwd
