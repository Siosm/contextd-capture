#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
from selinux import *

# ==========================================
# Define easement classes
# ==========================================
#
# SELinuxGraph class is inspired from the official Python's graph design pattern. See http://www.python.org/doc/essays/graphs.html
#
# ==========================================

class ECLP:	# Enhanced Command Line Parser
	class Argument:
		def __init__(self, valued, optional):
			self.valued = valued
			self.optional = optional
		
		def isValued(self):
			return self.valued
		
		def isOptional(self):
			return self.optional
	
	class Command:
		def __init__(self, func, *args):
			self.function_pointer = func
			if args:
				self.argument_linker = list()
				command_arguments = iter(*args)
				while True:
					try:
						self.argument_linker.append(next(command_arguments))
					except StopIteration:
						break
			else:
				self.argument_linker = None
		
		def call(self, passed_arguments):
			if self.argument_linker:
				func_args = list()
				for arg in self.argument_linker:
					func_args.append(passed_arguments[arg])
				
				self.function_pointer(*func_args)
			else:
				self.function_pointer()
	
	def __init__(self):
		self.argument_list = list()		# Stores all known arguments.
		self.command_list = dict()		# Stores all known commands but link each of them to a command ID (a sum of each argument's index in the list below)
		self.argument_parser = dict()	# Dynamicaly built to store all known switches and linked them to their respective argument (an argument could have multiple switches)
		self.command_parser = dict()	# Dynamicaly built to linked a command ID to a function's pointer
		
		#self.declareCommand(self.help, "--help")
	
	def help(self):
		pass
	
	def declareCommand(self, func, *switches):
		import inspect
		import copy
		func_args, func_varargs, func_varkw, func_defaults, func_kwonlyargs, func_kwonlydefaults, func_annotations = inspect.getfullargspec(func)
		for arg in func_args:
			if arg == "self": continue
			# index arguments
			if not arg in self.argument_list: self.argument_list.append(arg)
			
			# link switches to arguments
			if len(arg) > 1: key = "--%s" % arg
			else: key = "-%s" % arg
			self.argument_parser[key] = (arg, self.Argument(True, False))
			
		for sw in switches:
			# create argument based on given switch and index it
			if not sw in self.argument_list: self.argument_list.append(sw)
			self.argument_parser[sw] = (sw, self.Argument(False, False))
		
		# Registers command based on function's name
		if not func.__name__ in self.command_list:
			cmdId = 0
			all_args = copy.copy(func_args)
			all_args.extend(switches)
			command_arguments = iter(all_args)
			while True:
				try:
					arg = next(command_arguments)
					if arg == "self": continue
					cmdId += 1 << self.argument_list.index(arg)
				except ValueError:
					pass
				except StopIteration:
					break
			
			self.command_list[func.__name__] = cmdId
			self.command_parser[cmdId] = self.Command(func, func_args)
		
	def eval(self, cmd_line):
		#print(self.argument_list)
		#print(self.command_list)
		#print(self.argument_parser)
		#print(self.command_parser)
		
		passing_arguments = dict()
		cmdId = 0		# 0 means help function
		switches = iter(cmd_line)
		
		sw = next(switches)
		while True:
			try:
				arg, arg_object = self.argument_parser[sw]
				cmdId += 1 << self.argument_list.index(arg)
				
				sw = next(switches)
				if arg_object.isValued():
					while not sw.startswith("-"):
						passing_arguments.setdefault(arg, list()).append(sw)
						sw = next(switches)
			except KeyError:
				continue
			except StopIteration:
				break
		
		try:
			self.command_parser[cmdId].call(passing_arguments)
		except KeyError:
			pass

class SELinuxGraph(dict):	# TBD
	class ulist(list):
		def extend(self, item_list):
			for item in item_list:
				self.append(item)
		
		def append(self, item):
			if not item in self:
				list.append(self, item)
	
	def add_edge(self, source, target):
		self.setdefault(source, self.ulist()).append(target)
		if not target in self:
			self.__setitem__(target, self.ulist())
	
	def next(self, node):
		try:
			return self.__getitem__(node)
		except KeyError:
			return None
	
	# Methods for pathfinding
	# TODO

# ==========================================
# Define main class and main method
# ==========================================

class SELinuxOptimizer:
	def __init__(self, token_list):
		self.__tokens = token_list
	
	# ===== Define optimizer routines =====
	
	def selfize_rules(self):
		for rule in filter(lambda x: isinstance(x, SELinuxRule), self.__tokens):
			if rule.source == rule.target:
				rule.target = "self"
	
	def factorize_rules(self):
		known_rules = dict()
		to_delete = ulist()
		for rule in filter(lambda x: isinstance(x, SELinuxRule), self.__tokens):
			key = (rule.source, rule.target, repr_enums(rule.object_class))
			#print("\n\nKey = '%s'\nknown_rules = '%s'" % (key, known_rules))
			if key in known_rules:
				known_rules[key].add_attributes(rule.attributes)
				index = self.__tokens.index(rule)
				to_delete.append(index)
			else:
				known_rules[key] = rule
		
		to_delete.sort()	# Shouldn't be required but who knows ...
		while(to_delete):
			del self.__tokens[to_delete.pop()]
	
	def factorize_rule_attributes(self):
		# xor --> mutual exclusion --> symmetric_difference
		# xor --> exclusion --> difference
		# and --> intersection
		# or --> union
		
		for rule in filter(lambda x: isinstance(x, SELinuxRule), self.__tokens):
			attrs_set = set(map(lambda x: repr(x), rule.attributes))
			to_delete = ulist()
			to_add = ulist()
			for obj_perm_set in filter(lambda x: isinstance(x, SELinuxDefinePermSet), self.__tokens):
				perms_set = set(map(lambda x: repr(x), obj_perm_set.perms))
				inter = attrs_set.intersection(perms_set)
				if len(inter) == len(perms_set):
					to_delete.extend(perms_set)
					to_add.append(obj_perm_set.id)
			to_add.extend(attrs_set.difference(to_delete))
			rule.attributes = ulist()
			rule.attributes.extend(map(lambda x: SELinuxAttribute(x), to_add))
	
	def sort_rules(self, by_source = True):
		def sort_func(item):
			if isinstance(item, SELinuxMetaRule): return "%s %s %s" % (item.source, item.__class__, item.target)
			else: return repr(item)
		
		self.__tokens.sort(key=sort_func)
	
	# ===== The class really starts here =====
	
	def show(self):
		for token in self.__tokens:
			print(token)
			
	def set_has_name(self, _set, name):
		found = False
		for t in _set:
			if name==t.name:
				found = True
		return found
	
	def to_file(self, outputfile, permissiveDomains=list()):
		file = open(outputfile, "w")
		
		types = set();
		roles = set();
		attributes = set();
		classes = set();
		booleans = set();
		
		# Write module header (SELinuxModule)
		for token in filter(lambda x: isinstance(x, SELinuxModule), self.__tokens):
			file.write("%s\n" % token)
			
		#
		if len(permissiveDomains) > 0:
			file.write("\n\n# ====================\n# Permissive domains\n# ====================\n")
			for d in permissiveDomains:
				file.write("permissive %s;\n" % d)
		
		# Write types (SELinuxType)
		file.write("\n\n# ====================\n# Types\n# ====================\n")
		for token in filter(lambda x: isinstance(x, SELinuxType), self.__tokens):
			file.write("%s\n" % token)
			types.add(token);
		
		# Write roles (SELinuxRole)
		file.write("\n\n# ====================\n# Roles\n# ====================\n")
		for token in filter(lambda x: isinstance(x, SELinuxRole), self.__tokens):
			file.write("%s\n" % token)
			roles.add(token);
			
		# Require
		file.write("\n\n# ====================\n# Require\n# ====================\n")
		req_types = set();
		req_roles = set();
		req_attributes = set();
		req_classes = set();
		req_booleans = set();
		# Get all the requirements
		for token in self.__tokens:
			if isinstance(token, SELinuxBaseElement) or isinstance(token, SELinuxInterface) or isinstance(token, SELinuxMetaRule):
				for t in token.req_types():
					if t != "self" and not self.set_has_name(types, t):
						req_types.add(t);
				for r in token.req_roles():
					if not self.set_has_name(roles, r):
						req_roles.add(r);
				for a in token.req_attributes():
					if not self.set_has_name(attributes, a):
						req_attributes.add(a);
				for c in token.req_classes():
					if not self.set_has_name(classes, c):
						req_classes.add(c);
				for b in token.req_booleans():
					if not self.set_has_name(booleans, b):
						req_booleans.add(b);
			
		# Write them down
		if len(req_types) > 0:
			file.write("require { type %s;}" % ",".join(req_types))
		if len(req_roles) > 0:
			file.write("require { roles %s;}" % ",".join(req_roles))
		if len(req_attributes) > 0:
			file.write("require { attributes %s;}" % ",".join(req_attributes))
		if len(req_classes) > 0:
			file.write("require { class %s;}" % ",".join(req_classes))
		if len(req_booleans) > 0:
			file.write("require { bool %s;}" % ",".join(req_booleans))
		
		# Write interfaces (SELinuxInterface)
		file.write("\n\n# ====================\n# Interfaces\n# ====================\n")
		for token in filter(lambda x: isinstance(x, SELinuxInterface), self.__tokens):
			file.write("%s\n" % token)
		
		# Write rules (SELinuxMetaRule)
		file.write("\n\n# ====================\n# Rules\n# ====================\n")
		source_type_list = list()
		for token in filter(lambda x: isinstance(x, SELinuxMetaRule), self.__tokens):
			if not token.source in source_type_list:
				file.write("\n# ==> %s\n" % token.source)
				source_type_list.append(token.source)
			file.write("%s\n" % token)
		
		file.close()

def __main__(cmd_line):
	def test_scanner(inputfile):
		scanner = SELinuxScanner()
		
		for file in inputfile:
			f = open(file, "r")
			scanner.test(f.read())
			f.close()
	
	def test_parser(inputfile):
		scanner = SELinuxScanner()
		parser = SELinuxParser(scanner, bnf="selinux.bnf")
		
		for file in inputfile:
			f = open(file, "r")
			parser.test(f.read())
			parser.clean()
			f.close()
	
	def optimize(inputfile, outputfile):
		scanner = SELinuxScanner()
		parser = SELinuxParser(scanner)
		
		for file in inputfile:
			f = open(file, "r")
			tokens = parser.parse(f.read())
			f.close()
		
		optimizer = SELinuxOptimizer(tokens)
		optimizer.selfize_rules()
		optimizer.factorize_rules()
		optimizer.factorize_rule_attributes()
		optimizer.sort_rules()
		optimizer.to_file(outputfile)
		
	def connard():
		scanner = SELinuxScanner()
		parser = SELinuxParser(scanner, bnf="selinux.bnf")
		
		f = open("firefox.te", "r")
		tokens = parser.parse(f.read())
		f.close()
		optimizer = SELinuxOptimizer(tokens)
		optimizer.selfize_rules()
		optimizer.factorize_rules()
		optimizer.factorize_rule_attributes()
		optimizer.sort_rules()
		optimizer.to_file("poulpe.te")
	
	
	#def test():
	#	test_str = "policy_module(perso,1.0.0)"
	#	
	#	scanner = SELinuxScanner()
	#	parser = SELinuxParser(scanner, method="LALR", debug=True, write_tables=0)
	#	parser.test(test_str)
	
	#connard()
	
	cmd = ECLP()
	
	cmd.declareCommand(test_scanner, "-s")
	cmd.declareCommand(test_parser, "-p")
	cmd.declareCommand(optimize)
	#cmd.declareCommand(test, "--test")
	
	cmd.eval(cmd_line)

if (__name__ == "__main__") and (sys.argv[1:]):
	__main__(sys.argv[1:])