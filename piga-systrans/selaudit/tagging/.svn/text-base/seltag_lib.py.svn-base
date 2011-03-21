#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os, re
import xml.dom.minidom
from xml.dom.minidom import Node

def matchTagToPolicy(tag, te_policies, fc_policies):
	tags = set()
	if tag.hasAttribute('tags'):
		tags = set(tag.attributes['tags'].value.split(None))
	else:
		return tags
	
	# Try to match every te rule
	for te in tag.getElementsByTagName("te"):
		matched = False
		te_src = '.*'
		te_dst = '.*'
		te_attr = set()
		
		if te.hasAttribute('src'):
			te_src = te.attributes['src'].value
		if te.hasAttribute('dst'):
			te_dst = te.attributes['dst'].value
		if te.hasAttribute('attributes'):
			te_attr = set(te.attributes['attributes'].value.split(None))
		
		#print("te_src=%s, te_dst=%s, te_attr=%s" % (te_src, te_dst, te_attr))
		for pr in te_policies:
			if not re.match(te_src, pr['src']):
				continue
			if not re.match(te_dst, pr['dst']):
				continue
			if not te_attr.issubset(pr['attr']):
				continue
			matched = True
		
		if not matched:
			return set()
			
	# Try to match every fc rule
	for fc in tag.getElementsByTagName("fc"):
		matched = False
		fc_path = '.*'
		fc_type = '.*'
		fc_context = '.*'
		evenIfCommented = False
		
		if fc.hasAttribute('path'):
			fc_path = fc.attributes['path'].value
		if fc.hasAttribute('type'):
			fc_type = fc.attributes['type'].value
		if fc.hasAttribute('context'):
			fc_context = fc.attributes['context'].value
		if fc.hasAttribute('evenIfCommented'):
			evenIfCommented = fc.attributes['evenIfCommented'].value.lower() == "true"
		
		for pr in fc_policies:
			if pr['commented'] and not evenIfCommented:
				continue
			if not re.match(fc_path, pr['path']):
				continue
			if not re.match(fc_type, pr['type']):
				continue
			if not re.match(fc_context, pr['context']):
				continue
			matched = True
		
		if not matched:
			return set()
	
	# If we are here, then we have matched every te & every fc
	return tags

def tag_policy(conf, policy_te, policy_fc):
	tags = set()
	
	# Read the te policy
	if not os.path.exists(policy_te):
		return set(), "The te policy file %s doesn't exist\n" % policy_te
	te_policy_file = open("%s" % policy_te, "r")
	
	# split the te policy file
	te_policies = []
	for line in te_policy_file:
		m = re.match(r"allow (?P<src>\S+) (?P<dst>\S+) {? *(?P<attributes>[^}]*) *}? *;", line)
		if m:
			pol = dict()
			pol['src'] = m.groupdict()['src']
			pol['dst'] = m.groupdict()['dst']
			pol['attr'] = set(m.groupdict()['attributes'].split(None))
			# print("Found rule: src='%s', dst='%s', attr='%s'" % (pol['src'], pol['src'], pol['attr']))
			te_policies.append(pol)
			
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

	doc = xml.dom.minidom.parse(conf)
	if doc.documentElement.tagName != "tagging_rules":
		return set(), "The configuration file is invalid. It should start by the tag 'tagging_rules'."
	
	for tag in doc.getElementsByTagName("tag"):
		tags.update(matchTagToPolicy(tag, te_policies, fc_policies))
	
	return tags, "Success"

def tag_compare(conf, policy1_te, policy1_fc, policy2_te, policy2_fc):
	tags1,ret = tag_policy(conf, policy1_te, policy1_fc)
	if len(tags1) == 0:
		return set(), set(), 0, ret
		
	tags2,ret = tag_policy(conf, policy2_te, policy2_fc)
	if len(tags1) == 0:
		return set(), set(), 0, ret
		
	union = tags1.union(tags2)
	inter = tags1.intersection(tags2)
	
	return union, inter, len(inter)*100/len(union), "Success"
