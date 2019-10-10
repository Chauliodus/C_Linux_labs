#!/bin/bash

#var USAGE = ""

echo "Usage: ./lab1.sh path/to/work <name pattern> <file name> <count of 1st level dir-s> <count of 2st level dir-s> <count of files>"

cd $1
#ls | grep DIRS >> a.out
#if (a.out > 0) echo "YES"
mkdir DIRS
cd DIRS

for ((i = 0; i < $4; i++))
do
	mkdir $2$i 
	cd $2$i
	for((j = 0; j < $5; j++))
	do
		mkdir $2$i.$j
		cd $2$i.$j
		for((k = 0; k < $6; k++))
		do
			touch $3$i.$j.$k 
		done
		cd ../ 
	done
	cd ../
done

pwd
