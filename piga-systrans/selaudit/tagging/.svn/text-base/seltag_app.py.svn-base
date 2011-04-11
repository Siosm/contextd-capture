#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys, getopt
from seltag_lib import *

def usage():
	print('Usage: %s [--conf=/path/to/the/conf] policy.tmp policy.fc' % sys.argv[0], file=sys.stderr)
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
	policy = ''
	
	# Get the parameters
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			show_help()
			return 0
		
		elif opt in ("-c", "--conf"):
			conf = arg
	
	# if there are no args left, then an error happened
	if len(args) < 2:
		usage()
	else:
		policy_te = args[len(args)-2]
		policy_fc = args[len(args)-1]
		
		tags,err = tag_policy(conf, policy_te, policy_fc)
		if tags == set():
			print("Tagging policy failed with error '%s'\n" % err)
		else:
			print("%s" % ' '.join(sorted(tags)))

if __name__ == '__main__': 
	main()
