#!/usr/bin/python
'''
purpose:
    creates resources.h and resources.rc
    for vstforx.
'''
import sys
import argparse
import time
import os.path

resCounter = 100

def getResourceId(obj):
    if obj.has_key('resID'):
        return obj['resID']
    global resCounter
    x = resCounter
    obj['resID'] = x
    resCounter = resCounter + 10
    return x

def scanimages(_dir, images):
    print("scan for images in " + _dir + ":")
    for cd,sd,files in os.walk(_dir):
        for x in files:
            f = os.path.basename(cd) + '/' + x
            print(f)
            images[f] = {"path": os.path.abspath(x)}
    print("=============================")
    print(str(len(f)) + " images found.")

def createInitResourcesCpp(files, dstpath):
    f = open('initResourceMap._cpp', "r")
    iTxt = f.read()
    f.close()
    iTxt = iTxt.replace('$$$DATE$$$', time.asctime())
    calls = ""
    for x in files.keys():
        resID = getResourceId(files[x])
        calls = calls + '\tregisterF("%s", %i);\n' % (x, resID)
    iTxt = iTxt.replace('$$$REGISTER_CALLS$$$', calls)
    fname = dstpath + '/' + 'initResourceMap.cpp'
    f = open(fname, "w")
    f.write(iTxt)
    f.close()
    print(fname + " wrote.")

def createResourcesRc(files, dstpath):
    f = open('resources._rc', "r")
    iTxt = f.read()
    f.close()
    iTxt = iTxt.replace('$$$DATE$$$', time.asctime())
    calls = ""
    for x in files.keys():
        resID = getResourceId(files[x])
        calls = calls + '%s\tSAMBAG_CUSTOM\t"%s"\n' % (resID, files[x]['path'])
    iTxt = iTxt.replace('$$$RESOURCE_DEFS$$$', calls)
    fname = dstpath + '/' + 'resources.rc'
    f = open(fname, "w")
    f.write(iTxt)
    f.close()
    print(fname + " wrote.")



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='creates class')
    parser.add_argument('--imageindir','-i',  
	dest='indir', 
	help='the image in directory', 
	required=True)
    parser.add_argument('--out', '-o', 
	dest='outdir', 
	help='the outdir', 
	required=True)
    args = parser.parse_args()
    images={}
    scanimages(args.indir, images)
    createInitResourcesCpp(images, args.outdir)
    createResourcesRc(images, args.outdir)
