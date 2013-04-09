#!/usr/bin/env python

import sys
separator='\t'

def read_mapper_output(file):
  previous = None
  for line in file:
    line = line.rstrip("\n")
    tmp = line
    tmp = tmp.split()
    tmp = map(int,tmp)
    if tmp[2]>tmp[1]:
       if previous==line:
          print previous
       else:
          previous=line
#end of read_mapper_output


def main():
  read_mapper_output(sys.stdin)
#end of main

if __name__ == "__main__":
    main()
