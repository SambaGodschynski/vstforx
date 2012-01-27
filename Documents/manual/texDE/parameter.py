import re

f = open('parameter.txt', 'r')
text = f.read()
f.close

pat01 = re.compile('\s*((?P<name>.*?)\{(?P<values>.*?)\})', re.S)
pat02 = re.compile('\s*(.*?)\s*\|\s*(.*?)\s*\.\.\s*(.*)')
for x in pat01.finditer(text):
	name = x.group('name')
	values = x.group('values')
	for y in pat02.finditer(values):
		print '<%s><%s><%s>' % (y.group(1),  y.group(2), y.group(3))

