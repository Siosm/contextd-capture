#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys, getopt
from seltag_lib import *

def usage():
	print('Usage: %s [--conf=/path/to/the/conf] policy1.tmp policy1.fc policy2.tmp policy2.fc' % sys.argv[0], file=sys.stderr)
	exit(-1)

def main(argv=None):
	if argv is None:
		argv = sys.argv

	app_dir = os.path.dirname(os.path.realpath(argv[0])) + '/'
		
	# Check the given parameter names and get their values
	try:
		opts, args = getopt.getopt(argv[1:], "hc:",
		["help", "conf="])
	except(getopt.error) as msg:
		usage()
	
	# Params
	conf = app_dir+'seltag.conf'
	policy1 = ''
	policy1 = ''
	
	# Get the parameters
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			show_help()
			return 0
		
		elif opt in ("-c", "--conf"):
			conf = arg
	
	# if there are no args left, then an error happened
	if len(args) < 4:
		usage()
	else:
		policy1_te = args[len(args)-4]
		policy1_fc = args[len(args)-3]
		policy2_te = args[len(args)-2]
		policy2_fc = args[len(args)-1]
		
		union,inter,score,err = tag_compare(conf, policy1_te, policy1_fc, policy2_te, policy2_fc)
		if union == set():
			print("Camparing policies failed with error '%s'\n" % err)
		else:
			print("Score: %i%%" % score)
			print("Intersection: %s" % " ".join(sorted(inter)))
			print("Differences: %s" % " ".join(union.difference(sorted(inter))))
			# TODO: Print differences from both program point of view

if __name__ == '__main__': 
	main()
