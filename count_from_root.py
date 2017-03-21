#!/usr/bin/python


from subprocess import call
import os
from glob import glob
import sys
import time
import signal


def handler_function(arg1, arg2):
	# print "alarm!"
	print "files processed: " + str(files_counted)
	exit(0)


# root_dirs = ['/bin', '/usr', '/var']
root_dirs=glob('/*')
# root_dirs=[]
# print root_dirs
# print len(root_dirs)
counts = []

# Fill Counts
for i in range(0,len(root_dirs)):
	counts.append(0)

ct=0
for rd in root_dirs:
	for r, d, f in os.walk(rd):
		for j in f:
			print os.path.join(r,j)
			counts[ct] += 1
	# print "count it", ct
	# print root_dirs[ct] + " -> " + str(counts[ct])
	ct += 1

# print "total:", sum(counts)



# for i in range(0,len(root_dirs)):
	# print root_dirs[i] + " -> " + str(counts[i])

# get args:
# if (len(sys.argv) < 3):
	# print "useage: scramble_all.py <duration in seconds> <scramble>"
	# exit(0)
# dur = int(sys.argv[1])
# do_scram = int(sys.argv[2])

# Call func at alarm:
# signal.signal(signal.SIGALRM,handler_function)
# signal.alarm(dur) 

# files_counted = 0
# # Collect a list of dirs, filter out a couple bad ones:
# dirs = []
# paths = glob('/*/*/*/')
# for i in paths:
# 	if "/media" not in i and "/dev" not in i and "/proc" not in i:
# 		dirs.append(i)

# # Produce abspaths from the dirs for use in hashing:
# for d in dirs:
# 	files = glob(d+"*")
# 	if do_scram:
# 		call(["./scrambler", d])
# 	else:
# 		for f in files:
# 			# print f + " -- isdir says: " + str(os.path.isdir(f))
# 			if not os.path.isdir(f):
# 				# Give this teeth with 
# 				call(["md5sum", f])
# 				files_counted += 1