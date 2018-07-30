#!/bin/bash

g++ -pthread client.cpp -o client --std=c++11
t=1
for i in {101..200..8}
do
	fileName="results/$i"
#	for j in {1..2}
#	do
		echo "hey " $i $t
		./client $i $t 
		sleep 5 
#	done	
done
