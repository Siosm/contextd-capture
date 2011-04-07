#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Configuration
# Templates
basepolname = 'template/module'
base_transpol_name = 'template/temp_transition'
makefile_path = 'template/Makefile'

# Default value for the template variables
user_u_default  = 'user_u'
user_r_default  = 'user_r'
user_t_default = 'user_t'
module_domain_t_default_pattern = 'user_%modulename%_t'
module_exec_t_default_pattern = '%modulename%_exec_t'
module_tmp_domain_t_default_pattern = '%modulename%_tmp_t'
module_log_domain_t_default_pattern = '%modulename%_log_t'


# Selpolgen user
selpolgen_u_default = 'root'
selpolgen_r_default  = 'sysadm_r'
selpolgen_t_default = 'sysadm_t'

# Programs fullpath
semodule_path = '/usr/sbin/semodule'
make_path = '/usr/bin/make'
setfilecon_path = '/usr/sbin/setfilecon'
runcon_path = '/usr/bin/runcon'
audit2allow_path = '/usr/bin/audit2allow'
dmesg_path = '/bin/dmesg'
strace_path = '/usr/bin/strace'
ls_path = '/bin/ls'
setfiles_path = '/sbin/setfiles'

# /Configuration

# Import
import getopt
import re, string, sys
import os, signal
import glob
import subprocess
import shutil
import time
from pigi import *

# Global variables
verbosity = 0
wantToAbort = False

# functions
def log(priority, msg):
	if priority <= verbosity:
		print(msg)

def handler(signum, frame):
	global wantToAbort
	wantToAbort = True

def mkdir_p(path):
	if not os.path.exists (path):
		os.makedirs (path)
		
def getPolicyPath(module_name, extension=''):
	if len(extension) > 0:
		return "policies/%s/%s.%s" % (module_name, module_name, extension)
	else:
		return "policies/%s/" % module_name
		
def getTempModuleTransitionPath(module_name, extension=''):
	if len(extension) > 0:
		return "temp/%s/%s.%s" % (module_name, module_name, extension)
	else:
		return "temp/%s/" % module_name
	
def loadSELinuxModule(module_path_pp):
	proc = subprocess.Popen([semodule_path, '-i', module_path_pp], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		print("----\nError while loading the SELinux module '%s':\n<stdout>%s</stdout>\n<stderr>%s</stderr>\n----" % (module_path_pp, stdout, stderr), file=sys.stderr)
		return False
	else:
		return True

def unloadSELinuxModule(module_name):
	proc = subprocess.Popen([semodule_path, '-r', module_name], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		print("----\nError while unloading the SELinux module '%s':\n<stdout>%s</stdout>\n<stderr>%s</stderr>\n----" % (module_name, stdout, stderr), file=sys.stderr)
		return False
	else:
		return True

def reloadSELinuxModule(module_name):
	if unloadSELinuxModule(module_name):
		if loadSELinuxModule(getPolicyPath(module_name, "pp")):
			return True
		else:
			return False
	else:
		return False

def compileAndLoadSELinuxModule(module_dir):
	proc = subprocess.Popen([make_path, 'load'], cwd=module_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		print("----\nError while compiling and loading the module at '%s':\n<stdout>%s</stdout>\n<stderr>%s</stderr>\n----" % (module_dir, stdout, stderr), file=sys.stderr)
		return False
	else:
		return True

def generateBasePolicy(module_name, app_path, module_domain_t, module_tmp_domain_t, module_log_domain_t, module_exec_t, user_u, user_r, user_t, permissive_mode=False):
	#TODO add exceptions handling. It's fine for the moment as these exceptions are fatal for the program
	
	# Get the template
	template_te = open("%s.te" % basepolname, "r").read()
	template_if = open("%s.if" % basepolname, "r").read()
	template_fc = open("%s.fc" % basepolname, "r").read()
	
	if len(template_te) == 0:
		return ''
	
	if permissive_mode:
		template_te += "permissive ${module_domain_t};"
	
	# Replace the template variables by our values
	dico=dict({"module": module_name, "app_path": app_path, "module_domain_t": module_domain_t, "module_tmp_domain_t": module_tmp_domain_t, "module_log_domain_t": module_log_domain_t, "module_exec_t": module_exec_t, "user_u": user_u, "user_r": user_r, "user_t": user_t})
	
	for key in dico.keys():
		template_te=template_te.replace("${%s}" % key, dico[key])
		template_if=template_if.replace("${%s}" % key, dico[key])
		template_fc=template_fc.replace("${%s}" % key, dico[key])
	
	# Create a directory for the output module 
	mkdir_p(getPolicyPath(module_name, ""))
	
	# write the output module there
	file_te = open(getPolicyPath(module_name, "te"), "w").write(template_te)
	file_if = open(getPolicyPath(module_name, "if"), "w").write(template_if)
	file_fc = open(getPolicyPath(module_name, "fc"), "w").write(template_fc)
	
	# Copy the Makefile
	shutil.copyfile(makefile_path, "%sMakefile" % getPolicyPath(module_name, ""))
	
	return getPolicyPath(module_name)

def generateAuditPolicy(module_name, app_path, module_domain_t, module_exec_t, user_u, user_r, user_t):
	#TODO add exceptions handling. It's fine for the moment as these exceptions are fatal for the program
	module_name = "selpolgen-%s" % module_name
	
	# Get the template
	template_te = open("%s.te" % base_transpol_name, "r").read()
	template_if = open("%s.if" % base_transpol_name, "r").read()
	template_fc = open("%s.fc" % base_transpol_name, "r").read()
	
	if len(template_te) == 0:
		return ''
	
	# Replace the template variables by our values
	dico=dict({"module": module_name, "app_path": app_path, "module_domain_t": module_domain_t, "module_exec_t": module_exec_t, "user_u": user_u, "user_r": user_r, "user_t": user_t})
	
	for key in dico.keys():
		template_te=template_te.replace("${%s}" % key, dico[key])
		template_if=template_if.replace("${%s}" % key, dico[key])
		template_fc=template_fc.replace("${%s}" % key, dico[key])
		
	# Remove the directory for the output module
	try:
		shutil.rmtree(getTempModuleTransitionPath(module_name, ""))
	except:
		pass

	# Create a directory for the output module 
	mkdir_p(getTempModuleTransitionPath(module_name, ""))
	
	# write the output module there
	file_te = open(getTempModuleTransitionPath(module_name, "te"), "w").write(template_te)
	file_if = open(getTempModuleTransitionPath(module_name, "if"), "w").write(template_if)
	file_fc = open(getTempModuleTransitionPath(module_name, "fc"), "w").write(template_fc)
	
	# Copy the Makefile
	shutil.copyfile(makefile_path, "%sMakefile" % getTempModuleTransitionPath(module_name, ""))
	
	return getTempModuleTransitionPath(module_name)
	
def setFileSELinuxContext(user_u, role_r, type_t, filepath):
	context = '%s:%s:%s' % (user_u, role_r, type_t)
	proc = subprocess.Popen([setfilecon_path, context, filepath], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		print("Error while setting the context %s to the file '%s':\n<stdout>%s</stdout>\n<stderr>%s</stderr>" % (context, filepath, stdout, stderr), file=sys.stderr)
		return False
	else:
		return True

def getAudit2AllowRules(domain_t):
	rules = []
	
	proc = subprocess.Popen([audit2allow_path, "-d"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		print("Error while auditing:\n<stdout>%s</stdout>\n<stderr>%s</stderr>" % (stdout, stderr), file=sys.stderr)
		return rules
	
	lines=stdout.splitlines()
	
	log(2, "audit2allow output (%i lines) is: '%s'" % (len(lines), stdout))
	
	store=False
	for line in lines:
		line = line.decode()
		log(2, "line[:10] = '%s'" % (line[:10]))
		if line[:10] == "#=========":
			fields=line.split(" ")
			if fields[1] == domain_t:
				store = True
			else:
				store = False
		else:
			if store and len(line)>0:
				rules.append(line);
	
	return rules

def regeneratePolicy(policy_path, rules, permissive_domains = list()):
	# Add the lines to the policy
	template_te = open(policy_path, "a");
	#template_te.writelines(rules)
	for line in rules:
		template_te.write(line+"\n")
	template_te.close()
	
	# Parse it
	scanner = SELinuxScanner()
	parser = SELinuxParser(scanner)
	
	te_file = open(policy_path, "r")
	tokens = parser.parse(te_file.read())
	te_file.close()
	
	# Store it optimized
	optimizer = SELinuxOptimizer(tokens)
	optimizer.selfize_rules()
	optimizer.factorize_rules()
	optimizer.factorize_rule_attributes()
	optimizer.sort_rules()
	optimizer.to_file(policy_path, permissive_domains)
	
def updateAndReloadRules(module_name, module_domain_t, enforcingMode = True, forceReload=False):
	log(1, "Read the audit2allow output")
	rules = getAudit2AllowRules(module_domain_t)
	
	if forceReload or len(rules) > 0:
		log(0, "Add %i rules to %s and reload the policy" % (len(rules), getPolicyPath(module_name, "te")))
		if not enforcingMode:
			permissive_domains = [module_domain_t]
		else:
			permissive_domains = list()
		regeneratePolicy(getPolicyPath(module_name, "te"), rules, permissive_domains)
		
		# empty the logs
		dmesg = subprocess.Popen([dmesg_path, '-c'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		dmesg.communicate()
		
		# Load the new policy
		compileAndLoadSELinuxModule(getPolicyPath(module_name, ""))
		
	return len(rules)
	
def runApp(module_name, app_path, useStrace=False):
	if useStrace and os.path.exists(strace_path):
		print("Launch the application and trace it with strace")
		proc = subprocess.Popen([strace_path, '-e' 'trace=open,execve,mkdir', '-o', "%sstrace" % getTempModuleTransitionPath("selpolgen-%s" % module_name, ""), '-ff', '-F', app_path])
	else:
		print("Launch the application")
		proc = subprocess.Popen([app_path])
	
	# get the pid
	curPID = proc.pid
	
	return proc
	
def askToRunApp(app_path, domain_t, audit_fc=False):
	deleteFileList(["/tmp/selinux-audit"])
	
	print("\n****** Entering the auditing loop ******")
	
	if audit_fc:
		print("The application you are auditing will first be launched in a permissive mode, be sure to use all the functionnalities before quitting it.\n")
	
	print("Please launch this command in the domain %s: %s" % (domain_t, "selaudit_user.sh %s" % app_path))

def getFileCon(filepath):
	proc = subprocess.Popen([ls_path, '-Z', filepath], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = proc.communicate()
	
	if proc.returncode != 0:
		# print("Error while getting the context of the file '%s':\n<stdout>%s</stdout>\n<stderr>%s</stderr>" % (filepath, stdout, stderr), file=sys.stderr)
		return "<Context not found>"
	
	fields = str(stdout, "utf-8").split(' ')
	
	log(2, "getFileCon('%s') = '%s'" % (filepath, fields[0]))
	
	return fields[0]

def deleteFileList(to_be_deleted):
	for f in to_be_deleted:
		try:
			if os.path.isfile(f) or os.path.islink(f):
				os.remove(f)
			elif os.path.isdir(f):
				shutil.rmtree(f)
		except Exception as inst:
			print("deleteFileList: Caught exception %s: %s" % (type(inst), inst))
			pass

def escape_re(re):
	re = re.replace(".", "\\.");
	re = re.replace("(", "\\)");
	re = re.replace(")", "\\)");
	re = re.replace("|", "\\|");
	re = re.replace("^", "\\^");
	re = re.replace("*", "\\*");
	re = re.replace("+", "\\+");
	re = re.replace("?", "\\?");
	
	return re

def auditStraceLogs(module_name, dir_path="/tmp/selinux-audit/", saveResults=True):
	# dir_path = getTempModuleTransitionPath("selpolgen-%s" % module_name, "")
	
	execves = dict()
	mkdirs = dict()
	opens = dict()
	libs = dict()
	shms = dict()
	failed = list()
	
	to_be_deleted = list()
	
	# Read all the logs
	log_files = glob.glob("%s/strace*" % dir_path)
	for log in log_files:
		f = open(log, "r")
		
		for line in f:
			m = re.match(r"(?P<function>\w+) *\((?P<params>.*)\) *= *(?P<result>.*)", line)
			if m:
				args = m.group('params').split(', ')
				
				if not m.group('result').startswith("-1"):
					line = "%s(%s)" % (m.group('function'), ','.join(args))
					
					m2 = re.match(r"\"(.*)\"", args[0])
					if m2:
						filepath = m2.group(1)
						if m.group('function') == "open":
							if args[1].find('O_CREAT') != -1 or args[1].find('O_WRONLY') != -1:
								to_be_deleted.append(filepath)
							
							# Is the file a standard library ?
							stdlib = re.match(r"/(usr/)?lib/[^/]+", filepath)
							
							if filepath.startswith('/dev/shm'):
								if filepath not in shms:
									shms[filepath] = list()
								if line not in shms[filepath]:
									shms[filepath].append(line)
							elif stdlib:
								if filepath not in opens:
									libs[filepath] = list()
								if line not in libs[filepath]:
									libs[filepath].append(line)
							else:
								if filepath not in opens:
									opens[filepath] = list()
								if line not in opens[filepath]:
									opens[filepath].append(line)
						elif  m.group('function') == "mkdir":
							if filepath not in mkdirs:
								mkdirs[filepath] = list()
							if line not in mkdirs[filepath]:
								mkdirs[filepath].append(line)
								to_be_deleted.append(filepath)
						elif m.group('function') == "execve":
							if filepath not in execves:
								execves[filepath] = list()
							if line not in execves[filepath]:
								execves[filepath].append(line)
				else:
					line = "%s(%s)" % (m.group('function'), ','.join(args))
		
		f.close()
		
	# Delete all the strace files
	deleteFileList(log_files);
	
	if saveResults:
		# We have the logs, sorted by type and by path, generate the output file
		fc_file = open(getPolicyPath(module_name, "fc"), "a")
		
		fc_file.write("\n\n\n# **** Mkdir ****\n")
		mkdir_keys = mkdirs.keys()
		for dir_path in sorted(mkdir_keys):
			# Write all the interactions with this file
			for call in mkdirs[dir_path]:
				fc_file.write("# %s\n" % call)
			
			# Propose a rule
			fc_file.write("#%s/(.*/)?		%s\n\n" % (escape_re(dir_path), getFileCon(dir_path)))
		
		fc_file.write("\n\n\n# **** Execve ****\n")
		execve_keys = execves.keys()
		for exe_path in sorted(execve_keys):
			# Write all the interactions with this file
			for call in execves[exe_path]:
				fc_file.write("# %s\n" % call)
			
			# Propose a rule
			fc_file.write("#%s	--	%s\n\n" % (escape_re(exe_path), getFileCon(exe_path)))
			
		fc_file.write("\n\n\n# **** Open ****\n")
		open_keys = opens.keys()
		for open_path in sorted(open_keys):
			# Write all the interactions with this file
			for call in opens[open_path]:
				fc_file.write("# %s\n" % call)
			
			# Propose a rule
			fc_file.write("#%s	--	%s\n\n" % (escape_re(open_path), getFileCon(open_path)))
			
		fc_file.write("\n\n\n# **** Standard libraries ****\n")
		libs_keys = libs.keys()
		for lib_path in sorted(libs_keys):
			# Write all the interactions with this file
			for call in libs[lib_path]:
				fc_file.write("# %s\n" % call)
			
			# Propose a rule
			fc_file.write("#%s	--	%s\n\n" % (escape_re(lib_path), getFileCon(lib_path)))
			
		fc_file.write("\n\n\n# **** SHM ****\n")
		shms_keys = shms.keys()
		for shm_path in sorted(shms_keys):
			# Write all the interactions with this file
			for call in shms[shm_path]:
				fc_file.write("# %s\n" % call)
			
			# Propose a rule
			fc_file.write("#%s	--	%s\n\n" % (escape_re(shm_path), getFileCon(shm_path)))
	
	# Delete all the created files
	deleteFileList(to_be_deleted);

def parseFCFile(policy_fc):
	# Read the fc policy
	if not os.path.exists(policy_fc):
		return set(), "The fc policy file %s doesn't exist\n" % policy_fc
	fc_policy_file = open("%s" % policy_fc, "r")
	
	# split the fc policy file
	fc_policies = []
	for line in fc_policy_file:
		m = re.match(r"\s*(?P<comment>#)?(?P<path>\\?/\S+)\s+(?P<type>\S+)?\s+(?P<context>\S+)", line)
		if m:
			pol = dict()
			pol['path'] = m.groupdict()['path']
			pol['type'] = m.groupdict()['type']
			pol['context'] = m.groupdict()['context']
			pol['commented'] = m.groupdict()['comment']=="#"
			
			if (pol['type'] == None):
				pol['type'] = ''
			#print("Found rule: comment = '%s' path='%s', type='%s', context='%s'" % (pol['commented'], pol['path'], pol['type'], pol['context']))
			fc_policies.append(pol)
			
	return fc_policies
	
def addFCContextsToTE(policy_fc, policy_te):
	# Read the te policy
	if not os.path.exists(policy_te):
		return set(), "The te policy file %s doesn't exist\n" % policy_fc
	te_policy_file = open("%s" % policy_te, "a")
	
	fc_policies = parseFCFile(policy_fc)
	
	for policy in fc_policies:
		if not policy['commented']:
			print("got context %s\n" % policy['context'])
			te_policy_file.write("type %s;\nfiles_type(%s);\n" % (policy['context'], policy['context']))
		
	te_policy_file.close()

def editFiles(filepathes):
	editor_path = os.getenv('EDITOR')
	
	if not editor_path:
		print('The $EDITOR environement variable is not set.\nWhich editor would you like to use ?')
		editor = input('')
		os.environ['EDITOR'] = editor
	
	params = [editor_path]
	params.extend(filepathes)
	proc = subprocess.Popen(params)
	proc.communicate()
	
	return proc.returncode == 0
	
def willingToQuit():
	print("\nThe system is currently learning a SELinux security policy.")
	print("Deciding to stop it now means you have successfully tested all the functionnalities of the software you are auditing.")
	print("\nAre you sure you want to stop it ? (y/N)")
	
	answer=input('')
	if answer in ('y', 'Y', 'Yes', 'yes'):
		return True
	else:
		return False

def startAuditing(module_name, app_path, module_domain_t, module_tmp_domain_t, module_log_domain_t, module_exec_t, user_u, user_r, user_t, audit_fc, reuse):
	# Re-route signals to the launched process
	signal.signal(signal.SIGINT, handler)
	signal.signal(signal.SIGTERM, handler)
	signal.signal(signal.SIGHUP, handler)
	
	global wantToAbort
	wantToAbort = False
	
	if not reuse:
		# Get a base policy and load it
		print("Generate the base policy and load it")
		base_policy=generateBasePolicy(module_name, app_path, module_domain_t, module_tmp_domain_t, module_log_domain_t, module_exec_t, user_u, user_r, user_t, audit_fc)
		if not compileAndLoadSELinuxModule(base_policy):
			return False
	else:
		if not os.path.exists(getPolicyPath(module_name, "te")):
			print("The module %s doesn't exist." % module_name)
			return
		
	# Create a policy for selpolgen.py so as when it launches the audited program, the audited program will transit to the right domain
	print("Generate the sysadm policy to launch the application in the right context")
	temp_policy=generateAuditPolicy(module_name, app_path, module_domain_t, module_exec_t, user_u, user_r, user_t)
	if not compileAndLoadSELinuxModule(temp_policy):
		return False
	
	# Set the app context on the disc
	print("Set the application file's new context")
	setFileSELinuxContext("system_u", "object_r", module_exec_t, app_path)
	
	# run the application
	askToRunApp(app_path, user_t, audit_fc);
	
	if audit_fc:
		isRunning = True
		while isRunning :
			if wantToAbort:
				if willingToQuit():
						sys.exit(0)
				else:
						wantToAbort = False
				
			time.sleep(0.1)
			
			# remove the lock if the file exists
			if os.path.exists("/tmp/selinux-audit/lock"):
				isRunning = False
				
		# Propose some file constraints
		print("Update the fc file, this may take a while");
		auditStraceLogs(module_name)
		
		# Regenerate the policy
		updateAndReloadRules(module_name, module_domain_t, enforcingMode = True, forceReload=True)
						
		# let the application start again
		os.remove("/tmp/selinux-audit/lock")
		
		print("FC Labelling done\n")

	
	print("Start the TE learning loop")
	
	# learning loop
	nbRulesAddedSinceLastExecution = 0
	execStart = time.time()
	zeroRuleLoopCount = 0
	while True:
		if wantToAbort:
			if willingToQuit():
				break
			else:
				wantToAbort = False
		
		time.sleep(0.1)

		nbRulesAddedSinceLastExecution += updateAndReloadRules(module_name, module_domain_t)
		
		# remove the lock if the file exists
		if os.path.exists("/tmp/selinux-audit/lock"):
			if nbRulesAddedSinceLastExecution > 0:
				auditStraceLogs(module_name, dir_path="/tmp/selinux-audit/", saveResults=False)
				zeroRuleLoopCount = 0
			elif time.time()-execStart > 2.0 or zeroRuleLoopCount > 5:
				print("\n**********\nNo rules have been added during the execution of this audit instance.")
				print("Have you tested every use case allowed for the application ? (y/N)")
				answer=input('')
				print("**********")
				if answer in ('y', 'Y', 'Yes', 'yes'):
					break
				zeroRuleLoopCount = 0
			else:
				zeroRuleLoopCount = zeroRuleLoopCount + 1
				print("The instance didn't generate any rules but carry on nevertheless (%s/5)" % zeroRuleLoopCount)
			
			nbRulesAddedSinceLastExecution = 0
			execStart = time.time()
			os.remove("/tmp/selinux-audit/lock");
		
	print("\nThe final policy can be found at %s" % getPolicyPath(module_name, ""))

class Usage(Exception):
	def __init__(self, msg):
		Exception.__init__(self)
		self.msg = msg
		
def show_help():
	print("Help:\n")
	print("-h or --help                 : This help message")
	print("-m or --module               : The name of the SELinux module you would like to create (mandatory)")
	print("-u or --user_u               : The SELinux user who will execute the application")
	print("-r or --user_r               : The SELinux role who will execute the application")
	print("-t or --user_t               : The SELinux type who will execute the application")
	print("-d or --module_domain_t      : The domain in which the audited application will be executed")
	print("-e or --module_exec_t        : The file label that will be given to the application")
	print("-t or --module_tmp_domain_t  : The file label that will be given to the application's tmp files")
	print("-l or --module_log_domain_t  : The file label that will be given to the application's log files")
	print("-f or --no_fc_pass           : Do not fill the fc file. Learning the policy will take one iteration less")
	print("-p or --reuse_policy         : Re-use a pre-existing policy and learn what's new")

def main(argv=None):
	if argv is None:
		argv = sys.argv

	cwd = os.path.dirname(os.path.realpath(argv[0])) + '/'
	os.chdir(cwd)

	try:
		# Check the given parameter names and get their values
		try:
			opts, args = getopt.getopt(argv[1:], "hvm:u:r:t:d:e:t:l:fp",
			["help", "verbose", "module=", "user_u=", "user_r=", "user_t=", "module_domain_t=", "module_exec_t=", "module_tmp_domain_t=", "module_log_domain_t=", "no_fc_pass", "reuse_policy"])
		except(getopt.error) as msg:
			print("Argument parsing error: %s" % msg)
			raise Usage(msg)
		
		# Params
		module_name = ''
		module_domain_t = ''
		module_exec_t = ''
		module_tmp_domain_t = ''
		module_log_domain_t = ''
		audit_fc = True
		app_fullpath = ''
		user_u = user_u_default
		user_r = user_r_default
		user_t = user_t_default
		reuse = False
		
		# Get the parameters
		for opt, arg in opts:
			if opt in ("-h", "--help"):
				show_help()
				return 0
			
			elif opt in ("-v", "--verbose"):
				verbosity += 1
			
			elif opt in ("-m", "--module"):
				module_name = arg
			
			elif opt in ("-u", "--user_u"):
				user_u = arg
			
			elif opt in ("-r", "--user_r"):
				user_r = arg
				
			elif opt in ("-t", "--user_t"):
				user_t = arg
				
			elif opt in ("-e", "--module_exec_t"):
				module_exec_t = arg
				
			elif opt in ("-d", "--module_domain_t"):
				module_domain_t = arg
			
			elif opt in ("-t", "--module_tmp_domain_t"):
				module_tmp_domain_t = arg
				
			elif opt in ("-l", "--module_log_domain_t"):
				module_log_domain_t = arg
			
			elif opt in ("-f", "--no_fc_pass"):
				audit_fc = False
			
			elif opt in ("-p", "--reuse_policy"):
				reuse = True
		
		# if there are no args left, then an error happened
		if len(args) == 0  or module_name == '':
			print('Usage: %s [options] -m module_name filepath' % sys.argv[0], file=sys.stderr)
		else:
			# Get the fullpath
			app_fullpath = args[len(args)-1]
			
			# Set the default value for module_domain_t & module_exec_t if there were not set by the user
			if module_domain_t == '':
				module_domain_t = module_domain_t_default_pattern.replace("%modulename%", module_name)
			if module_exec_t == '':
				module_exec_t = module_exec_t_default_pattern.replace("%modulename%", module_name)
			if module_tmp_domain_t == '':
				module_tmp_domain_t = module_tmp_domain_t_default_pattern.replace("%modulename%", module_name)
			if module_log_domain_t == '':
				module_log_domain_t = module_log_domain_t_default_pattern.replace("%modulename%", module_name)
				
			# Let's recap to the user what he has chosen.
			print('You are about to create a SELinux module for the application')
			print('')
			print('Here is the summary of how it will be created:')
			print('	Module name (-m): %s' % module_name)
			print('	Application path: \'%s\'' % app_fullpath)
			print('	Will be labelled as ():():(-e):%s:%s:%s' % ('system_u', 'object_r', module_exec_t))
			print('	Be executed by (-u):(-r):(-t): %s:%s:%s' % (user_u, user_r, user_t))
			print('	Jailed in the domain (-d): %s' % module_domain_t)
			print('	Tmp file\'s domain is (-t): %s' % module_tmp_domain_t)
			print('	Log file\'s domain is (-l): %s' % module_log_domain_t)
			print('	Do not audit the fc file (bad practice!) (-f): %s' % (not audit_fc))
			print('	Re-use an existing policy (-p): %s' % (reuse))
			print('')
			print('Do you agree with that ? (Y/n)')
			answer=input('')
			
			if answer in ('', 'y', 'Y', 'Yes', 'yes'):
				startAuditing(module_name, app_fullpath, module_domain_t, module_tmp_domain_t, module_log_domain_t, module_exec_t, user_u, user_r, user_t, audit_fc, reuse)
			else:
				return 0

	except(Usage) as err:
		print('%s: %s' % (sys.argv[0], err.msg), file=sys.stderr)
		print('For a list of available options, use "%s --help"'\
		% sys.argv[0], file=sys.stderr)
		return -1

if __name__ == '__main__': 
	main()
