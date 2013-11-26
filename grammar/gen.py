#!/usr/bin/env python
# coding: utf-8

# Needs two trailing newlines

import sys
import fileinput
import re
from pprint import pprint
from collections import defaultdict
from itertools import groupby, chain
from operator import itemgetter

DEBUG = False

def terminal(t):
	# This also matches 'EPSILON'
	return re.match(r'^[A-Z_]+$', t)

def make_first_fun(productions):
	first = set([])
	call_first = set([])

	for p in productions:
		if terminal(p[0]):
			first.add(p[0])
		else:
			call_first.add(p[0])

	def first_fun():
		f = first
		for c in call_first:
			# print "CALLING FIRST OF", c
			f.update(first_funs[c]())
		return f

	return first_fun

def make_follow_fun(nt, terms):
	def follow_fun():
		# print "FINDING FOLLOW OF", nt, "TERMS", terms
		follow = set([])
		for f in terms:
			if type(f) == str:
				follow.add(f)
			else:
				follow.update(f())
		return follow

	return follow_fun


def first_following_epsilon(beta, A, B):
	# print "FINDING FOLLOW OF", B, "WITH", beta
	follow = set([])
	for s in beta:
		if follow and "EPSILON" not in follow:
			return follow
		elif "EPSILON" in follow:
			follow.remove("EPSILON")

		if terminal(s):
			# print "FOUND TERMNAL", s
			follow.add(s)
			return follow
		else:
			follow.update(first_funs[s]())

	if A is None:
		# print "A is None"
		return follow
	elif follow and "EPSILON" not in follow:
		return follow
	elif "EPSILON" in follow:
		follow.remove("EPSILON")

	if A != B:
		# print "CALLING FOLLOW of", A
		try:
			follow.update(follow_funs[A]())
		except RuntimeError as e:
			if e.message == 'maximum recursion depth exceeded while calling a Python object':
				# print "MAXIMUM RECURSION DEPTH EXCEEDED"
				pass
			else:
				raise e

	return follow

def call_first_following_epsilon(beta, A, B):
	def first():
		return first_following_epsilon(beta, A, B)

	return first

if len(sys.argv) < 2:
	sys.exit("USAGE: " + sys.argv[0] + "start_symbol, [grammar]")

start_symbol = sys.argv[1]

productions = {}

A = None
current_productions = []

first_funs = {}
follow_terms = defaultdict(list)
follow_funs = {}
production_order = []

for line in fileinput.input(sys.argv[2:]):
	line = line.rstrip('\n')
	if line == '':
		first_funs[A] = make_first_fun(current_productions)
		productions[A] = current_productions
		production_order.append(A)
		A = None
		current_productions = []
	elif re.match(r'^[a-z0-9_]+$', line):
		A = line
		# print 'NT', A
	elif re.match(r'^	→ .*', line):
		production = line.split(' ')[1:]
		current_productions.append(production)
		# print 'PRODUCTION', production

		for i, B in enumerate(production):
			if not terminal(B):
				# print "WILL FIND FOLLOW OF", B, "WITH", production[i+1:]
				follow_terms[B].append(call_first_following_epsilon(production[i+1:], A, B))
	else:
		print "ERROR", line


if DEBUG:
	for nt, f in first_funs.iteritems():
		print "FIRST of", nt, "=", list(f())
	print

follow_terms[start_symbol].append("EOF")

for nt, terms in follow_terms.iteritems():
	follow_funs[nt] = make_follow_fun(nt, terms)

if DEBUG:
	for nt, f in follow_funs.iteritems():
		print "FOLLOW of", nt, " =", list(f())
	print

parse_table = defaultdict(lambda: defaultdict(list))

for A, current_productions in productions.iteritems():
	for alpha in current_productions:
		first = first_following_epsilon(alpha, A=None, B=None)
		for a in first:
			if a != "EPSILON":
				parse_table[A][a].append(alpha)
		if "EPSILON" in first:
			follow = follow_funs[A]()
			for b in follow:
				parse_table[A][b].append(alpha)

if DEBUG:
	for nt in follow_funs:
		print "PARSE TABLE ROW OF", nt
		for a, alphas in parse_table[nt].iteritems():
			print a, "=", alphas
			if len(alphas) > 1:
				print "MULTIPLE PRODUCTIONS FOR SYMBOL", a
		print

for nt in production_order:
	print "type parse_{nt}();".format(nt=nt)
	print

for nt in production_order:
	first = first_funs[nt]()
	follow = follow_funs[nt]()

	print "type parse_{nt}() ".format(nt=nt) + "{"
	print "\t// first({nt}): {first}".format(nt=nt, first=', '.join(first))
	print "\t// follow({nt}): {follow}".format(nt=nt, follow=', '.join(follow))
	for a, alphas in parse_table[nt].iteritems():
		if len(alphas) > 1:
			print "\t// MULTIPLE PRODUCTIONS FOR SYMBOL:", a
	print

	print '\tDEBUG_PRINT(("Parsing {nt}\\n"));'.format(nt=nt)
	print

	print "\tTOKEN_TYPE expected[] = {{{set}}};".format(set=', '.join(t + '_TYPE' for t in parse_table[nt]))
	# print "\tTOKEN_TYPE synch_set[] = {{{set}}};".format(set=', '.join(t + '_TYPE' for t in set(['EOF']).union(first).union(follow) if t != 'EPSILON'))
	print "\tTOKEN_TYPE synch_set[] = {{{set}}};".format(set=', '.join(t + '_TYPE' for t in set(['EOF']).union(follow)))
	print

	print "\tswitch(tok.type) {"

	expanded = sorted(chain.from_iterable(((k, v) for v in vs) for k, vs in parse_table[nt].iteritems()), key=itemgetter(1))
	for alpha, a_grouped in groupby(expanded, key=itemgetter(1)):
		# print alpha
		# print list(a_grouped)
		for a, alpha_same in a_grouped:
			print "\tcase {a}_TYPE: ".format(a=a)

		if alpha[0] != "EPSILON":
			print "\t\t{"
		print "\t\t// Grammar production: " + ' '.join(alpha)
		for e in alpha:
			if e == "EPSILON":
				print "\t\t// Epslion production"
			elif terminal(e):
				print "\t\tif (!match({t}_TYPE))".format(t=e)
				print "\t\t\tgoto synch;"
			else:
				print "\t\ttype {nt}_type = parse_{nt}();".format(nt=e)
		parsed_nonterminals = [e for e in alpha if not terminal(e)]
		if parsed_nonterminals:
			print "\t\tif (" + ' || '.join(nt + "_type == ERROR_STAR || " + nt + "_type == ERROR" for nt in parsed_nonterminals) + ")"
			print "\t\t\treturn ERROR;"
		print "\t\treturn NONE;"
		if alpha[0] != "EPSILON":
			print "\t\t}"

	print "\tdefault:"
	print '\t\twrite_listing_synerr(lineno, tok, "{nt}", expected, sizeof(expected)/sizeof(expected[0]));'.format(nt=nt)
	print "\t\tbreak;"
	print "\t}"
	print
	print "synch:"
	print "\tsynch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));"
	print "\treturn ERROR_STAR;"
	print "}"
	print

print

