#!/bin/bash

numberOfThreads=$1
bit=$2
if [ $# -ne 2 ] 
then
  echo "ERROR: Requires 1 arguments: $0 [numberOfThreads] [key_size_in_bits]"
  exit
fi

#speed up
echo -e "Speed up test, from 1 to $1 threads, $bit bits key  ========================== \n"
for ((i=1; i<=$1; i*=2))
do
    echo "$i threads:"
	java BruteForceDES $i $bit
	echo -e "\n"
done

echo -e "Scale up test, from 1 to $1 threads,========================== \n"
for ((i=1; i<=$1; i*=2))
do
	echo "$i threads, $bit bits key:"
    java BruteForceDES $i $bit
	echo ""
	bit=$(($bit+1))
done
