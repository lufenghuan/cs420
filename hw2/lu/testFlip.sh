#!/bin/bash
numberOfThreads=$1

if [ $# -ne 1 ]
then
  echo "ERROR: Requires 1 arguments: $0 [numberOfThreads]"
  exit
fi
numOfFlips=1000000000

#speed up test
echo -e "Speed up test, from 1 to $1 threads, fliping $numOfFlips coins \n"

for (( i=1 ; i<=$1; i*=2))
do
	echo "$i threads:"
	java CoinFlip $i $numOfFlips
	echo -e "\n"
done

#scale up test
echo -e  "Scale up test, from 1 to $1 threads. \n"

for ((i = 1; i<=$1; i*=2))
do
	num=$(($numOfFlips * $i))
	echo "$i threads,   $num :"
	java CoinFlip $i $num
	echo ""
done


#Measure startup cost
echo -e "startup cost \n"
for((i=1;i<=20;i++))
do
	java CoinFlip 500 0	
done
