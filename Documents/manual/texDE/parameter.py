import re

f = open('parameter.txt', 'r')
text = f.read()
f.close

def param_name(x):
	return '\\paramName{%s}' % x

def param_min(x):
	return '\\paramMin{%s}' % x

def param_max(x):
	return '\\paramMax{%s}' % x

pat01 = re.compile('\s*((?P<name>.*?)\{(?P<values>.*?)\})', re.S)
pat02 = re.compile('\s*(.*?)\s*\|\s*(.*?)\s*\.\.\s*(.*)')

for x in pat01.finditer(text):
	name = x.group('name')
	values = x.group('values')
	print '%s:' % name
	print '\\bParameterlist'
	for y in pat02.finditer(values):
		print '\t%s%s ... %s' % (param_name(y.group(1)),  param_min(y.group(2)), param_max(y.group(3)))
	print '\\eParameterlist'
