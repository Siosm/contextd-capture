# -*- coding: utf-8 -*-
# ==========================================
# Define base classes
# ==========================================
#
# Those classes are used to create object which are returns by parser
#
# ==========================================

class BNFRule:
	def __init__(self, id, rules, use_with_ply=False):
		self.id = id
		self.rules = rules
		self.ply_aware = use_with_ply		# Define a boolean to make this BNF rule PLY comaptible
	
	def __repr__(self):
		if self.ply_aware:
			return "%s : %s" % (self.id, " | ".join(self.rules))
		else:
			return "<%s> := %s" % (self.id, " | ".join(self.rules))

# ==========================================
# Define analyzer classes	
# ==========================================

class BNFScanner:	# BNF tokenizer	(aka LEX)
	# ===== This header prepares lexer =====
	
	# Define reserved words
	reserved = dict()
	
	# Define all simple tokens
	tokens = [
		# basic tokens, punctuation
		"COLON",
		"BAR",
		"EQUAL",
		"LT",
		"GT",
		"IDENTIFIER",
	] + list(reserved.values())
	
	# Describe simple tokens
	t_COLON     = r"\:"
	t_BAR       = r"\|"
	t_EQUAL     = r"\="
	t_LT		= r"<"
	t_GT		= r">"
	
	# Describe new lines
	def t_newline(self, t):
		r"\n+"
		t.lexer.lineno += len(t.value)
	
	# Declare ignored tokens
	t_ignore						=	" \t"
	t_ignore_comments				=	"\#.*\n"

	# Define what to do when given token is unknown
	def t_error(self, t):
		print("Illegal character '%s'" % t.value[0])
		t.lexer.skip(1)
	
	# Describe literals
	def t_IDENTIFIER(self, t):
		r"[a-zA-Z0-9_]+"
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

class BNFParser:	# BNF Parser (aka YACC)
	# ===== TODO list =====
	# --> Write following AST's branches (@see refparser.py BNF):
	#	--> p_statement
	#		- token
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
		"statement	:	rule"
		p[0] = p[1]
	
	def p_empty(self, p):
		"empty	:"
		pass
	
	# ===== This part defines AST's main branches =====
	
	def p_rule(self, p):
		"""rule	:	LT identifier GT COLON EQUAL names
				|	LT identifier GT COLON EQUAL"""
		if len(p) == 6:
			p[0] = BNFRule(p[2], list())
		else:
			p[0] = BNFRule(p[2], p[6])
	
	# ===== This part defines AST's sub branches =====
	
	# ===== This part defines AST's leafs =====
	
	def p_names(self, p):
		"""names	:	identifier_list
					|	names BAR identifier_list"""
		if len(p) == 2:
			p[0] = [p[1]]
		else:
			p[0] = p[1]
			p[0].append(p[3])
	
	def p_identifier_list(self, p):
		"""identifier_list	:	identifier
							|	identifier_list identifier"""
		if len(p) == 2:
			p[0] = p[1]
		else:
			p[0] = "%s %s" % (p[1], p[2])
	
	def p_identifier(self, p):
		"identifier	:	IDENTIFIER"
		p[0] = p[1]
	
	# Error rule to handle syntax errors
	def p_error(self, p):
		print("***\nSyntax error on input! --> line %d: %s [type=%s]\n***" % (p.lineno, p.value, p.type))
	
	# ===== The class start here =====

	# Build parser
	def __init__(self, tokenizer, **kwargs):
		import ply.yacc as yacc
		self.__statements = list()
		self.__tokenizer = tokenizer
		self.tokens = tokenizer.tokens
		self.__parser = yacc.yacc(module=self, tabmodule="%s_parsetab" % self.__class__.__name__, **kwargs)
	
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
