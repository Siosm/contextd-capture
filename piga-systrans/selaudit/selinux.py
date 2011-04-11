# -*- coding: utf-8 -*-
from bnf import *
import itertools

# ==========================================
# Define easement functions and classes
# ==========================================
#
#
#
# ==========================================

def repr_enums(enum, **kwargs):	# Static Method
	if len(enum) > 1:
		return "{ %s }" % " ".join(sorted(map(lambda x: repr(x), enum), **kwargs))
	else:
		return "%s" % enum[0]

class ulist(list):
	def __init__(self, lst=list()):
		list.__init__(self, set(lst))
	
	def extend(self, lst):
		return list.extend(self, list(set(lst).difference(self)))
		
	def append(self, elt):
		return self.extend([elt])

# ==========================================
# Define base classes
# ==========================================
#
# Those classes are used to create object which are returned by the parser
#
# TODO list:
#	--> Check SELinux classes and their __repr__ methods
#	--> Create SELinuxClass class and patch parser (about rules)
#
# ==========================================

# Define base class
class SELinuxBaseElement:
	def __init__(self, identifier):
		self.identifier = identifier
	
	def __repr__(self):
		return "%s" % self.identifier
	
	def req_types(self):
		return set()
	
	def req_roles(self):
		return set()
	
	def req_attributes(self):
		return set()
	
	def req_classes(self):
		return set()
	
	def req_booleans(self):
		return set()

class SELinuxAttribute(SELinuxBaseElement):
	pass

class SELinuxClass(SELinuxBaseElement):
	pass

class SELinuxMetaRule:	# Easement class: Not aimed to be instanciated
	def __init__(self, context_source, context_target):
		self.source = context_source
		self.target = context_target
		
	def req_types(self):
		s = set()
		s.add(self.source)
		s.add(self.target)
		return s
	
	def req_roles(self):
		return set()
	
	def req_attributes(self):
		return set()
		
	def req_classes(self):
		return set()
		
	def req_booleans(self):
		return set()

# Define rule class
class SELinuxRule(SELinuxMetaRule):
	def __init__(self, source_type, target_type, object_class, attribute_list):
		SELinuxMetaRule.__init__(self, source_type, target_type)
		
		if not isinstance(attribute_list, list):
			attribute_list = [attribute_list]
		if not isinstance(object_class, list):
			object_class = [object_class]
		
		self.attributes = ulist()
		self.attributes.extend(map(lambda x: SELinuxAttribute(x), attribute_list))
		
		self.object_class = ulist()
		self.object_class.extend(map(lambda x: SELinuxClass(x), object_class))
	
	def __to_list(self, obj):
		if not isinstance(obj, (list, tuple)):
			return [obj]
		return obj
	
	def add_attributes(self, attribute_list):
		attributes = map(lambda x: repr(x), self.attributes)
		
		for attr in self.__to_list(attribute_list):
			if not repr(attr) in attributes:
				self.attributes.append(attr)
				
	def req_types(self):
		s = set()
		s.add(self.source)
		s.add(self.target)
		return s
	
	def req_roles(self):
		return set()
	
	def req_attributes(self):
		attr = set()
		"""for a in self.attributes:
			print("Add req attribute %s" % a)
			attr.add(a);"""
		return attr
		
	def req_classes(self):
		return set()
		
	def req_booleans(self):
		return set()
		
	def del_attributes(self, attribute_list):
		attributes = map(lambda x: repr(x), self.__to_list(attribute_list))
		
		for attr in self.attributes:
			if repr(attr) in attributes:
				try:
					self.attributes.remove(attr)
				except ValueError:
					continue
	
	def has_attributes(self, attribute_list):
		inter = set(map(lambda x: repr(x), self.__to_list(attribute_list))).intersection(map(lambda x: repr(x), self.attributes))
		
		return len(inter) == len(self.__to_list(attribute_list))

class SELinuxRuleAllow(SELinuxRule):
	def __repr__(self):
		return "allow %s %s:%s %s;" % (self.source, self.target, repr_enums(self.object_class), repr_enums(self.attributes))

class SELinuxRuleDeny(SELinuxRule):
	def __repr__(self):
		return "neverallow %s %s:%s %s;" % (self.source, self.target, repr_enums(self.object_class), repr_enums(self.attributes))

class SELinuxRuleAudit(SELinuxRule):
	def __repr__(self):
		return "auditallow %s %s:%s %s;" % (self.source, repr_enums(self.object_class), repr_enums(self.attributes), self.target)

class SELinuxRuleNoAudit(SELinuxRule):
	def __repr__(self):
		return "dontaudit %s %s:%s %s;" % (self.source, repr_enums(self.target), repr_enums(self.object_class), repr_enums(self.attributes))

# Define type's rule class
class SELinuxTypeRule(SELinuxMetaRule):
	def __init__(self, source_type, target_type, object_class, destination):
		SELinuxMetaRule.__init__(self, source_type, target_type)
		
		if not isinstance(object_class, list):
			object_class = [object_class]
		
		self.object_class = ulist()
		self.object_class.extend(map(lambda x: SELinuxClass(x), object_class))
		
		self.destination = destination
	
class SELinuxTypeTransition(SELinuxTypeRule):
	def __repr__(self):
		return "type_transition %s %s:%s %s;" % (self.source, self.target, repr_enums(self.object_class), self.destination)

class SELinuxTypeChange(SELinuxTypeRule):
	def __repr__(self):
		return "type_change %s %s:%s %s;" % (self.source, self.target, repr_enums(self.object_class), self.destination)

class SELinuxTypeMember(SELinuxTypeRule):
	def __repr__(self):
		return "type_member %s %s:%s %s;" % (self.source, self.target, repr_enums(self.object_class), self.destination)

# Define selinux define class
class SELinuxDefine:
	def __init__(self, identifier, selinux_object_list):
		self.id = identifier
		self.objects = selinux_object_list

class SELinuxDefineM4Value(SELinuxDefine):
	def __init__(self, identifier):
		SELinuxDefine.__init__(self, identifier, None)
		
	def __repr__(self):
		return "define(`%s')" % self.id

class SELinuxDefinePermSet(SELinuxDefine):
	def __init__(self, identifier, permissions, refpolicywarn=None):
		lst = list()
		for perm in permissions:
			lst.append(SELinuxAttribute(perm))
		
		if refpolicywarn:
			lst.append(refpolicywarn)
		
		SELinuxDefine.__init__(self, SELinuxAttribute(identifier), lst)
	
	def __getattr__(self, name):
		if name == "perms":
			return tuple(filter(lambda x: isinstance(x, SELinuxAttribute), self.objects))
		elif name == "refpolicywarn":
			return tuple(filter(lambda x: isinstance(x, SELinuxRefPolicyWarn), self.objects))
		else:
			raise(AttributeError, name)
	
	def __repr__(self):
		permissions = self.perms
		refpolicywarn = self.refpolicywarn
		
		str = "define(`%s', `" % self.id
		
		if len(permissions) == 1:
			str += "{ %s }" % permissions[0]
		else:
			str += "%s" % repr_enums(permissions)
		
		if refpolicywarn:
			str += " %s" % refpolicywarn[0]
		
		str += "')"
		return str

# Define module class
class SELinuxModule:
	def __init__(self, module, version):
		self.module = module
		self.version = version
	
	def __repr__(self):
		return "module %s %s;" % (self.module, self.version)

class SELinuxPolicyModule(SELinuxModule):
	def __repr__(self):
		return "policy_module(%s, %s)" % (self.module, self.version)

# Define interface class
class SELinuxInterface:
	def __init__(self, name, inputs):
		self.name = name
		self.inputs = inputs
		
		self.r_types = set()
		self.r_roles = set()
		for i in inputs:
			if i.endswith("_t"):
				self.r_types.add(i)
			elif i.endswith("_r"):
				self.r_roles.add(i)
	
	def req_types(self):
		return self.r_types
	
	def req_roles(self):
		return self.r_roles
	
	def req_attributes(self):
		return set()
	
	def req_classes(self):
		return set()
	
	def req_booleans(self):
		return set()

class SELinuxInterfaceCall(SELinuxInterface):
	def __repr__(self):
		return "%s(%s);" % (self.name, ", ".join(self.inputs))

# Define other selinux class
class SELinuxRefPolicyWarn:
	def __init__(self, text):
		self.text = text
	
	def __repr__(self):
		return "refpolicywarn(`%s')" % self.text

# Define role
class SELinuxRole(SELinuxBaseElement):
	def __init__(self, name, types=list()):
		self.name = name
		self.types = types
	
	def __repr__(self):
		if len(self.types) == 0:
			return "role %s;" % self.name
		else:
			#tmp = map(lambda x: repr(x), self.types)
			return "role %s types %s;" % (self.name, ", ".join(self.types))
		
# Define type
class SELinuxType(SELinuxBaseElement):
	def __init__(self, name, aliases=list(), attributes=list()):
		self.name = name
		self.aliases = aliases
		self.attributes = attributes
	
	def __repr__(self):
		aliases = ''
		attributes = ''
		
		if len(self.aliases) > 0:
			tmp = map(lambda x: repr(x), self.aliases)
			aliases = "alias {%s}" % (" ".join(tmp))
		
		if len(self.attributes) > 0:
			tmp = map(lambda x: repr(x), self.attributes)
			attributes = ", %s" % (", ".join(tmp))
		
		return "type %s %s%s;" % (self.name, aliases, attributes)
	
class SELinuxPermissiveDomain(SELinuxBaseElement):
	def __init__(self, domains):
		self.domains = domains
	
	def __repr__(self):
		tmp = map(lambda x: repr(x), self.domains)
		return "permissive %s;" % (", ".join(tmp))
	
	def req_types(self):
		s = set()
		for domain in self.domains:
			s.add(domain)
		return s
	
# ==========================================
# Define analyzer classes	
# ==========================================
#
# These classes has been inspired by the work of Karl MacMillan <kmacmillan@mentalrootkit.com>, Red Hat's contributor.
# SELinuxScanner and SELinuxParser classes are based on the refparser.py source code from the Debian's package "sepolgen"
#
# ==========================================

class SELinuxScanner:	# SELinux tokenizer	(aka LEX)
	# ===== This header prepares lexer =====
	
	# Define reserved words
	reserved = {
		# module
		"module"		:	"MODULE",
		"policy_module"	:	"POLICY_MODULE",
		"require"		:	"REQUIRE",
		
		# flask
		"sid"			:	"SID",
		"genfscon"		:	"GENFSCON",
		"fs_use_xattr"	:	"FS_USE_XATTR",
		"fs_use_trans"	:	"FS_USE_TRANS",
		"fs_use_task"	:	"FS_USE_TASK",
		"portcon"		:	"PORTCON",
		"nodecon"		:	"NODECON",
		"netifcon"		:	"NETIFCON",
		"pirqcon"		:	"PIRQCON",
		"iomemcon"		:	"IOMEMCON",
		"ioportcon"		:	"IOPORTCON",
		"pcidevicecon"	:	"PCIDEVICECON",
		
		# object classes
		"class"	:	"CLASS",
		
		# types and attributes
		"typeattribute"	:	"TYPEATTRIBUTE",
		"type"			:	"TYPE",
		"attribute"		:	"ATTRIBUTE",
		"alias"			:	"ALIAS",
		"typealias"		:	"TYPEALIAS",
		
		# conditional policy
		"bool"	:	"BOOL",
		"true"	:	"TRUE",
		"false"	:	"FALSE",
		"if"	:	"IF",
		"else"	:	"ELSE",
		
		# users and roles
		"role"	:	"ROLE",
		"types"	:	"TYPES",
		
		# types
		"type"	:	"TYPE",
		"alias"	:	"ALIAS",
		
		# require
		"gen_require"	:	"GEN_REQ",
		"require"	:	"REQUIRE",
		
		# permissive
		"permissive"	:	"PERMISSIVE",
		
		# rules
		"allow"				:	"ALLOW",
		"dontaudit"			:	"DONTAUDIT",
		"auditallow"		:	"AUDITALLOW",
		"neverallow"		:	"NEVERALLOW",
		"type_transition"	:	"TYPE_TRANSITION",
		"type_change"		:	"TYPE_CHANGE",
		"type_member"		:	"TYPE_MEMBER",
		"range_transition"	:	"RANGE_TRANSITION",
		"role_transition"	:	"ROLE_TRANSITION",
		
		# refpolicy keywords
		"optional_policy"	:	"OPT_POLICY",
		"interface"			:	"INTERFACE",
		"tunable_policy"	:	"TUNABLE_POLICY",
		"gen_require"		:	"GEN_REQ",
		"template"			:	"TEMPLATE",
		"gen_context"		:	"GEN_CONTEXT",
		"refpolicywarn"		:	"REFPOLICYWARN",
		
		# M4
		"ifelse"	:	"IFELSE",
		"ifndef"	:	"IFNDEF",
		"ifdef"		:	"IFDEF",
		"define"	:	"DEFINE"
	}
	
	# Define all simple tokens
	tokens = [
		# literals
		"TEXT",
		"IDENTIFIER",
		"NUMBER",
		"PATH",
		# basic tokens, punctuation
		"TICK",
		"SQUOTE",
		"LBRACE",
		"RBRACE",
		"SEMI",
		"COLON",
		"POINT",
		"LPAREN",
		"RPAREN",
		"COMMA",
		"MINUS",
		"TILDE",
		"ASTERISK",
		"AMP",
		"BAR",
		"EXPL",
		"EQUAL"
	] + list(reserved.values())
	
	# Describe simple tokens
	t_TICK      = r"\`"
	t_SQUOTE    = r"\'"
	t_LBRACE    = r"\{"
	t_RBRACE    = r"\}"
	t_SEMI      = r"\;+"
	t_COLON     = r"\:"
	t_POINT		= r"\."
	t_LPAREN    = r"\("
	t_RPAREN    = r"\)"
	t_COMMA     = r"\,"
	t_MINUS     = r"\-"
	t_TILDE     = r"\~"
	t_ASTERISK  = r"\*"
	t_AMP       = r"\&"
	t_BAR       = r"\|"
	t_EXPL      = r"\!"
	t_EQUAL     = r"\="
	t_NUMBER    = r"[0-9]+(\.[0-9]+)*"
	t_PATH      = r"/[a-zA-Z0-9)_\.\*/]*"
	
	# Describe new lines
	def t_newline(self, t):
		r"\n+"
		t.lexer.lineno += len(t.value)
	
	# Declare ignored tokens
	t_ignore						=	" \t"
	t_ignore_comments				=	"\#.*\n"
	t_ignore_m4comment				=	"dnl.*\n"
	#t_ignore_define_refpolicywarn	=	"define.*refpolicywarn\(.*\n"
	#t_ignore_refpolicywarn			=	"refpolicywarn\(.*\n"

	# Define what to do when given token is unknown
	def t_error(self, t):
		print("Illegal character '%s'" % t.value[0])
		t.lexer.skip(1)
	
	# Describe literals
	def t_refpolicywarn_TEXT(self, t):
		r"(?<=refpolicywarn\(\`).*?(?=\'\))"
		#import re
		#match = re.search( r"(?<=\(\`).*?(?=\'\))", t.value)
		#t.lexpos += match.start()
		#t.value = match.group()
		t.type = "TEXT"
		return t
	
	def t_IDENTIFIER(self, t):
		r"[a-zA-Z_\$][a-zA-Z0-9_\.\$\*]*"
		t.type = self.reserved.get(t.value, "IDENTIFIER")    # Check for reserved words
		return t
	
	# ===== The class start here =====
	
	# Build lexer
	def __init__(self, **kwargs):
		import ply.lex as lex
		self.lexer = lex.lex(module=self, **kwargs)
	
	# Tokenize datas and return lexer
	def tokenize(self, datas):
		self.lexer.input(datas)
		return self.lexer
	
	# Tokenize datas and print result
	def test(self, datas):
		for token in self.tokenize(datas):
			print(token)

class SELinuxParser:	# SELinux Parser (aka YACC)
	# ===== TODO list =====
	# --> Write following AST's branches (@see refparser.py BNF):
	#	--> p_statement
	#		- interface
	#		- template
	#
	# --> Write the following AST's sub branches (@see refparser.py BNF):
	#	--> p_policy:
	#		- optional_policy
	#		- tunable_policy
	#		- ifdef
	#		- ifelse
	#		- conditional
	#
	# --> Write the following AST's branches (@see refparser.py BNF):
	#	--> p_policy_stmt
	#		- gen_require (just parsed, not stored)
	#		- typeattribute_def
	#		- role_allow
	#		- typealias_def
	#		- attribute_def
	#		- range_transition_def
	#		- role_transition_def
	#		- bool
	#		- initial_sid
	#		- genfscon
	#		- fs_use
	#		- portcon
	#		- nodecon
	#		- netifcon
	#		- pirqcon
	#		- iomemcon
	#		- ioportcon
	#		- pcidevicecon
	#
	
	# ===== This part defines AST top level =====
	
	def p_statements(self, p):
		"""statements	:	statement
						|	statements statement
						|	empty"""
		if len(p) == 2:
			self.__statements.append(p[1])
		else:
			self.__statements.append(p[2])
	
	def p_statement(self, p):
		"""statement	:	policy_module
						|	module
						|	policy
						|	obj_perm_set
						|	refpolicywarn"""
		p[0] = p[1]
	
	def p_empty(self, p):
		"empty	:"
		pass
	
	# ===== This part defines AST's main branches =====
	
	def p_ref_policy_warn(self, p):
		"refpolicywarn	:	REFPOLICYWARN LPAREN TICK TEXT SQUOTE RPAREN"
		p[0] = self.selinux_refpolicywarn(p[4])
	
	def p_obj_perm_set(self, p):
		"""obj_perm_set	:	DEFINE LPAREN TICK identifier SQUOTE COMMA TICK names SQUOTE RPAREN
						|	DEFINE LPAREN TICK identifier SQUOTE COMMA TICK names refpolicywarn SQUOTE RPAREN"""
		if not isinstance(p[8], list):
			p[8] = [p[8]]
		
		if len(p) == 12:
			p[0] = self.selinux_object_permission_set(p[4], p[8], p[9])
		else:
			p[0] = self.selinux_object_permission_set(p[4], p[8], None)
	
	def p_policy(self, p):
		"policy	:	policy_stmt"
		p[0] = p[1]
	
	def p_policy_module(self, p):
		"policy_module	:	POLICY_MODULE LPAREN identifier COMMA number RPAREN"
		p[0] = self.selinux_policy_module(p[3], p[5])
	
	def p_module(self, p):
		"module : MODULE identifier number SEMI"
		p[0] = self.selinux_module(p[2], p[3])
	
	# ===== This part defines AST's sub branches =====
	
	def p_policy_stmt(self, p):
		"""policy_stmt	:	gen_require
						|	rule_def
						|	type_rule_def
						|	interface_call
						|	define
						|	role_def
						|	type_def
						|	permissive"""
		p[0] = p[1]
	
	def p_gen_require(self, p):
		"""gen_require	:	GEN_REQ LBRACE TICK requires SQUOTE RBRACE
						|	REQUIRE LBRACE requires RBRACE"""
		p[0]=None
	
	def p_requires(self, p):
		"""requires	:	require
					|	requires require"""
#					|	ifdef
#					|	requires ifdef
		p[0]=None
	
	def p_require(self, p):
		"""require	:	TYPE comma_list SEMI
					|	ROLE comma_list SEMI
					|	ATTRIBUTE comma_list SEMI
					|	CLASS comma_list SEMI
					|	BOOL comma_list SEMI"""
		p[0]=None
	
	def p_role_def(self, p):
		"""role_def	:	ROLE identifier TYPES comma_list SEMI
					|	ROLE identifier SEMI"""
		if len(p) == 4:
			p[0] = SELinuxRole(p[2])
		else:
			p[0] = SELinuxRole(p[2], p[4])
			
	def p_permissive(self, p):
		"""permissive	:	PERMISSIVE comma_list SEMI"""
		p[0] = SELinuxPermissiveDomain(p[2])
	
	def p_type_def(self, p):
		"""type_def	:	TYPE identifier COMMA comma_list SEMI
					|	TYPE identifier SEMI
					|	TYPE identifier ALIAS names SEMI
					|	TYPE identifier ALIAS names COMMA comma_list SEMI"""
		if len(p) == 4:
			p[0] = SELinuxType(p[2])
		elif len(p) == 6:
			#print("type, len(p) == %i, p==%s, p[3]='%s', p[4]='%s'" % (len(p), p[0], p[3], p[4]))
			if p[3] == ',':
				p[0] = SELinuxType(p[2], list(), p[4])
			elif p[3] == 'alias':
				p[0] = SELinuxType(p[2], p[4])
		elif len(p) == 8:
			p[0] = SELinuxType(p[2], p[4], p[6])
	
	# Define AST's branches about rules
	def p_rule_def(self, p):
		"""rule_def	:	rule_allow_def
					|	rule_deny_def
					|	rule_audit_def
					|	rule_dontaudit_def"""
		p[0] = p[1]
		
	def p_rule_allow_def(self, p):
		"rule_allow_def		:	ALLOW names names COLON names names SEMI"
		p[0] = self.selinux_rule_allow(p[2], p[3], p[5], p[6])
	
	def p_rule_deny_def(self, p):
		"rule_deny_def		:	NEVERALLOW names names COLON names names SEMI"
		p[0] = self.selinux_rule_deny(p[2], p[3], p[5], p[6])
	
	def p_rule_audit_def(self, p):
		"rule_audit_def		:	AUDITALLOW names names COLON names names SEMI"
		p[0] = self.selinux_rule_audit(p[2], p[6], p[3], p[5])
	
	def p_rule_dontaudit_def(self, p):
		"rule_dontaudit_def	:	DONTAUDIT names names COLON names names SEMI"
		if not isinstance(p[3], list):
			p[3] = [p[3]]
		
		p[0] = self.selinux_rule_dontaudit(p[2], p[3], p[5], p[6])
	
	# Define AST's branches about rules on types
	def p_type_rule_def(self, p):
		"""type_rule_def	:	type_rule_transition_def
							|	type_rule_change_def
							|	type_rule_member_def"""
		p[0] = p[1]
	
	def p_type_rule_transition_def(self, p):
		"type_rule_transition_def	:	TYPE_TRANSITION names names COLON names identifier SEMI"
		p[0] = self.selinux_rule_type_transition(p[2], p[3], p[5], p[6])
	
	def p_type_rule_change_def(self, p):
		"type_rule_change_def	:	TYPE_CHANGE names names COLON names identifier SEMI"
		p[0] = self.selinux_rule_type_change(p[2], p[3], p[5], p[6])
		
	def p_type_rule_member_def(self, p):
		"type_rule_member_def	:	TYPE_MEMBER names names COLON names identifier SEMI"
		p[0] = self.selinux_rule_type_member(p[2], p[3], p[5], p[6])
	
	# Define AST's branches about interfaces
	def p_interface_call(self, p):
		"""interface_call	:	identifier LPAREN interface_call_param_list RPAREN
							|	identifier LPAREN RPAREN
							|	identifier LPAREN interface_call_param_list RPAREN SEMI"""
		if len(p) == 3:
			p[0] = self.selinux_interface_call(p[1], list())
		else:
			p[0] = self.selinux_interface_call(p[1], p[3])
	
	# Define other AST's branches
	def p_define(self, p):
		"define	:	DEFINE RPAREN TICK identifier SQUOTE LPAREN"
		p[0] = self.selinux_define_m4value(p[4])
	
	# ===== This part defines AST's leafs =====
	
	def p_names(self, p):
		"""names	:	identifier
					|	nested_id_set
					|	asterisk
					|	TILDE identifier
					|	TILDE nested_id_set
					|	identifier MINUS identifier"""
		if len(p) == 2:
			p[0] = p[1]
		elif len(p) == 3:
			p[0] = p[2]
		else:
			p[0] = "%s -%s" % (p[1], p[3])
	
	def p_comma_list(self, p):
		"""comma_list	:	nested_id_list
						|	comma_list COMMA nested_id_list"""
		if not isinstance(p[1], list):
			p[1] = [p[1]]
			
		if len(p) == 2:
			p[0] = p[1]
		else:
			p[1].extend(p[3])
			p[0] = p[1]
	
	def p_nested_id_set(self, p):
		"nested_id_set	:	LBRACE nested_id_list RBRACE"
		p[0] = p[2]
	
	def p_nested_id_list(self, p):
		"""nested_id_list	:	nested_id_element
							|	nested_id_list nested_id_element"""
		if not isinstance(p[1], list):
			p[1] = [p[1]]
			
		if len(p) == 2:
			p[0] = p[1]
		else:
			p[0] = p[1]
			p[0].append(p[2])
	
	def p_nested_id_element(self, p):
		"""nested_id_element	:	identifier
								|	MINUS identifier
								|	nested_id_set"""
		if len(p) == 2:
			p[0] = p[1]
		else:
			p[0] = "-%s" % p[2]
			
	def p_interface_call_param_list(self, p):
		"""interface_call_param_list	:	interface_call_param
										|	interface_call_param_list COMMA interface_call_param"""
		if len(p) == 2:
			p[0] = [p[1]]
		else:
			p[0] = p[1]
			p[0].append(p[3])
		
	def p_interface_call_param(self, p):
		"""interface_call_param	:	identifier
								|	identifier MINUS identifier
								|	nested_id_set
								|	TRUE
								|	FALSE"""
		if len(p) == 2:
			p[0] = p[1]
		else:
			p[0] = [p[1]]
			p[0].append("-%s" % p[3])
	
	def p_sign(self, p):
		"""sign	:	MINUS
				|	TILDE
				|	AMP
				|	pipe
				|	equal
				|	punctuation_mark
				|	asterisk
				|	bracket"""
		p[0] = p[1]
	
	def p_number(self, p):
		"number	:	NUMBER"
		p[0] = p[1]
	
	def p_identifier(self, p):
		"identifier	:	IDENTIFIER"
		p[0] = p[1]

	def p_asterisk(self, p):
		"asterisk	:	ASTERISK"
		p[0] = p[1]
	
	def p_equal(self, p):
		"equal	:	EQUAL"
		p[0] = p[1]
	
	def p_pipe(self, p):
		"pipe	:	BAR"
		p[0] = p[1]
	
	def p_bracket(self, p):
		"""bracket	:	LBRACE
					|	RBRACE
					|	LPAREN
					|	RPAREN"""
		p[0] = p[1]
	
	def p_punctuation_mark(self, p):
		"""punctuation_mark	:	SEMI
							|	COLON
							|	COMMA
							|	EXPL
							|	POINT"""
		p[0] = p[1]
	
	# Error rule to handle syntax errors
	def p_error(self, p):
		print("***\nSyntax error on input! --> line %d: %s [type=%s]\n***" % (p.lineno, p.value, p.type))
	
	# ===== This part defines AST's callback functions (overridable) =====
	
	def selinux_refpolicywarn(self, text):
		return SELinuxRefPolicyWarn(text)
	
	def selinux_object_permission_set(self, identifier, permissions, refpolicywarn):
		return SELinuxDefinePermSet(identifier, permissions, refpolicywarn)
	
	def selinux_policy_module(self, module, version):
		return SELinuxPolicyModule(module, version)
	
	def selinux_module(self, module, version):
		return SELinuxModule(module, version)
	
	def selinux_rule_allow(self, source_type, target_type, object_class, permissions):
		return SELinuxRuleAllow(source_type, target_type, object_class, permissions)
	
	def selinux_rule_deny(self, source_type, target_type, object_class, permissions):
		return SELinuxRuleDeny(source_type, target_type, object_class, permissions)
	
	def selinux_rule_audit(self, source_type, target_type, object_class, permissions):
		return SELinuxRuleAudit(source_type, target_type, object_class, permissions)
	
	def selinux_rule_dontaudit(self, source_type, target_type, object_class, permissions):
		return SELinuxRuleNoAudit(source_type, target_type, object_class, permissions)
	
	def selinux_rule_type_transition(self, source, target, object_class, destination):
		return SELinuxTypeTransition(source, target, object_class, destination)
	
	def selinux_rule_type_change(self, source, target, object_class, destination):
		return SELinuxTypeChange(source, target, object_class, destination)
	
	def selinux_rule_type_member(self, source, target, object_class, destination):
		return SELinuxTypeMember(source, target, object_class, destination)
	
	def selinux_interface_call(self, name, inputs):
		return SELinuxInterfaceCall(name, inputs)
	
	def selinux_define_m4value(self, identifier):
		return SELinuxDefineM4Value(identifier)
	
	# ===== The class start here =====
	
	# Build parser
	def __init__(self, tokenizer, **kwargs):
		# Parse given *.bnf file and change p_*()'s doc on the fly
		try:
			#self.__import_bnf(kwargs["bnf"])
			del kwargs["bnf"]
		except KeyError:
			pass
		
		import ply.yacc as yacc
		self.__statements = list()
		self.__tokenizer = tokenizer
		self.tokens = tokenizer.tokens
		self.__parser = yacc.yacc(module=self, tabmodule="%s_parsetab" % self.__class__.__name__, **kwargs)
	
	
	def __import_bnf(self, bnf_file):
		# Parsing BNF file to extract rules and tokens
		try:
			file = open(bnf_file, "r")
		except IOError:
			return
		scanner = BNFScanner()
		parser = BNFParser(scanner, debug=0)
		bnf_rules = parser.parse(file.read())
		file.close()
		
		# Fixing __func__.__doc__ attributes
		for rule in bnf_rules:
			rule.ply_aware = True
			try:
				self.__getattribute__("p_%s" % rule.id).__func__.__doc__ = "%s" % rule
			except AttribtueError:
				continue
	
	# Parse datas and return result
	def parse(self, datas):
		self.__parser.parse(datas, lexer=self.__tokenizer.lexer)
		return self.__statements
	
	# Clean found token list
	def clean(self):
		self.__statements = list()
	
	# Tokenize and parse datas then print result
	def test(self, datas):
		for token in self.parse(datas):
			print(token)
		self.clean()
