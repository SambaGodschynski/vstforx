import sys
import os
import os.path
import re

inDir = "../src/"

add = """
IF(APPLE)
  SET(FRX_SOURCES ${FRX_MMSOURCES} )
ENDIF(APPLE)

add_library(frx_core ${FRX_SOURCES} ${VSTSDKSOURCE} ${VST3SDKSOURCE})
add_subdirectory(PluginApps)
add_subdirectory(mobile)

"""

ignoreDirs = (
    "TestFolders",
    "CMakeFiles",
    #".*test.*",
    ".*GuiStandaloneApp",
     ".*PluginApps",
     ".*mobile",
    ".*frxPluginDK"
)
ignoreFiles = (
    ".*win_Window.cpp",
    ".*Command.cpp",
    ".*RemoteChannelCounterpart",
    ".*cmlogo.png",
	".*FRX_FROZEN_.*"
)

class Walker():
    fHandler = None
    currDir = ""
    root = ""
    testSource = []
    mmsource = []
    resources = []
    source = {}
    def __init__(self, root):
        self.root = root
        
        for currDir, subDirs, files in os.walk(root):
               self.process(currDir, subDirs, files)
               
        self.fHandler = self.createCmakeFile()
        for x in self.source.keys():
            srcs = "FRX_%s" % x.upper()
            libn = "lib" + x
            self.writeList(srcs, self.source[x])
            self.writeLine("add_library(%s ${%s})" % (libn, srcs))
            self.writeLine("set(FRX_CLIBS ${FRX_CLIBS} %s)" % libn)
            self.writeLine("")
        self.writeList("FRX_MMSOURCES", self.mmsource)
        self.writeList("FRX_RESOURCES", self.resources)
        self.writeList("FRX_TESTSOURCES",self.testSource)
        self.writeLine(add)
        self.fHandler.close()

    def writeList(self, name, data):
        self.writeLine("SET ( %s" % (name))
        for x in data:
            x = x.replace("\\","/")
            self.writeLine("\t%s" % (x))
        self.writeLine(")")
        
    def writeLine(self, string):
        self.fHandler.write("%s\n" % (string))
        
    def createCmakeFile(self):
        return open(self.root+"/"+"CMakeLists.txt", "w")

    def passDir(self, _dir):
        for x in ignoreDirs:
            if re.match("%s" % (x), _dir, re.I):
                return False
        return True
    
    def passFile(self, f):
         for x in ignoreFiles:
            if re.match("%s" % (x), f, re.I):
                #print "ignore file: ", f
                return False
         return True     
    
    def processSubDirs(self, subDirs):
        pass


    def processFiles(self, files):
        for x in files:
            full = (os.path.relpath(self.currDir, self.root) +'/'+x).replace("\\", "/")
            name, ext = os.path.splitext(x)
            if ext == ".mm":
                if not self.passFile(x):
                    continue
                self.mmsource.append(full)
            if ext == ".png" and re.match("^images/.*", full):
                if not self.passFile(x):
                    continue
                self.resources.append("../"+full)
            if not re.match("\.cp{0,2}$", ext):
                continue
            if not self.passFile(x):
                continue
            if self.isTest(self.currDir):
                self.testSource.append(full)
            else:
                key = self.currDir.replace("../","")
                key =  re.sub("[^a-zA-Z0-9]", "_", key)
                if not self.source.has_key(key):
                    self.source[key] = []
                self.source[key].append(full)

    def isTest(self, _dir):
        if re.match(".*?test.*", _dir):
            return True
        return False
    
    def process(self, currDir, subDirs, files):
        if not self.passDir(currDir):
            #print "ignore dir: ", currDir
            return
        self.currDir = currDir
        #process
        self.processSubDirs(subDirs)
        self.processFiles(files)

w = Walker(inDir)


