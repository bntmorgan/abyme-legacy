#!/usr/bin/python

import sys
import getopt

def main(argv):
  start = 0
  step = 1
  format = "%016x "

  try:
    opts, args = getopt.getopt(argv, "s:p:f:")
  except getopt.GetoptError:
    print("%s -s <start-hex> -p <step-hex> [-f <format>]")
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-s':
      start = int(arg, 16)
    elif opt == "-p":
      step = int(arg, 16)
    elif opt == "-f":
      format = arg
  line = sys.stdin.readline()
  while line:
    print(format % start, line, sep="", end="")
    start = start + step
    line = sys.stdin.readline()
  
if __name__ == "__main__":
  main(sys.argv[1:])
