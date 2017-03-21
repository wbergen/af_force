#!/usr/bin/python
# Will Bergen - Python Wrapping for Scrambler (scram.c)

from subprocess import call
import os
from glob import glob
import sys
import time
import signal

# scramble_all.py <duration in seconds> <scramble>
# --| scramble 1: call scramble
# --| scramble 0: hash files


def handler_function(arg1, arg2):
	# print "alarm!"
	print "files processed: " + str(files_counted)
	exit(0)


# get args:
if (len(sys.argv) < 3):
	print "useage: scramble_all.py <duration in seconds> <scramble>"
	exit(0)
dur = int(sys.argv[1])
do_scram = int(sys.argv[2])

# Call func at alarm:
signal.signal(signal.SIGALRM,handler_function)
signal.alarm(dur) 

files_counted = 0
# Collect a list of dirs, filter out a couple bad ones:
dirs = []
paths = glob('/*/*/*/')
for i in paths:
	if "/media" not in i and "/dev" not in i and "/proc" not in i:
		dirs.append(i)

# Produce abspaths from the dirs for use in hashing:
for d in dirs:
	files = glob(d+"*")
	if do_scram:
		call(["./scrambler", d])
	else:
		for f in files:
			# print f + " -- isdir says: " + str(os.path.isdir(f))
			if not os.path.isdir(f):
				# Give this teeth with 
				call(["md5sum", f])
				files_counted += 1