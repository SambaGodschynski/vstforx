'''
purpose:
    creates new .h and .cpp file using .txt
    templates. Solution is quick and dirty, so the script
    has to be configured by variables below before
    executing.
'''

#io
name = 'RemoteChannel'
incpp = 'adapter.txt'
inh = 'adapterHeaderTmp.txt'
outdir = '../concreteAdapter/'

#replacments
repmap = {'$$$AUTHOR$$$': 'Johannes Unger',
          '$$$NAME$$$': name,
          '$$$ONCE$$$': 'FORX_' + name.upper() + '_H'
          }

################################################################################
def readFile(name):
    #returns file as string
    f = open(name, 'r')
    txt = f.read()
    f.close()
    return txt
def writeFile(name, txt):
    #writes txt into name
    f = open(name, 'w')
    f.write(txt)
    f.close()

def processFile( infile, outfile):
    txt = readFile(infile)
    for k, v in repmap.iteritems():
        txt = txt.replace(k, v);
    writeFile(outdir + outfile, txt)

processFile(inh, name + '.h')
processFile(incpp, name + '.cpp')
print('all done.')

