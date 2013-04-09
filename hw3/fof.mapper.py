#!/usr/bin/env python
"""A more advanced Mapper, using Python iterators and generators."""

import sys
separator='\t'

def read_input(file):
    for line in file:
        # split the line into words
        yield line.split()
#end of read_input

def process(str):
  for words in str:
    uid=words[0]
    words =  words[1:len(words)]
    for i in words:
      for j in words:
        if i!=j:
            print '%s %s %s' % (uid,i,j)
            print '%s %s %s' % (i,uid,j)


#end of process

def main():
    # input comes from STDIN (standard input)
    data = read_input(sys.stdin)
    process(data)
#end of main

if __name__ == "__main__":
    main()
