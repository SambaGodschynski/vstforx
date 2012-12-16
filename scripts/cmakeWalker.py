import sys
import os
import os.path
import re

inDir = "../src/"

add = """
SET ( FRX_STANDALONE
	GuiStandaloneApp/main.cpp
)

add_executable(standalone ${FRX_STANDALONE})
target_link_libraries (standalone frx_core ${FRX_CLIBS})

add_executable(unit_tests ${SAMBAG_TESTSOURCES})
target_link_libraries (unit_tests frx_core ${FRX_CLIBS})

SET (PlugSources ${PlugSources}
    ../VSTForx/VSTForx/VstForxPlugMain.cpp
    ../VSTForx/VSTForx/vstplug.def
)

add_library(vstforx SHARED ${PlugSources})
target_link_libraries (vstforx frx_core ${FRX_CLIBS})
"""

ignoreDirs = (
    "TestFolders",
    "CMakeFiles",
    #".*test.*",
    ".*GuiStandaloneApp",
    ".*mac.*"
)
ignoreFiles = (
    ".*win_Window.cpp",
    ".*Command.cpp"
)

class Walker():
    fHandler = None
    currDir = ""
    root = ""
    testSource = []
    source = []
    def __init__(self, root):
        self.root = root
        
        for currDir, subDirs, files in os.walk(root):
               self.process(currDir, subDirs, files)
               
        self.fHandler = self.createCmakeFile()
        self.writeList("FRX_SOURCES", self.source)
        self.writeList("SAMBAG_TESTSOURCES",self.testSource)
        self.writeLine("add_library(frx_core ${FRX_SOURCES} ${VSTSDKSOURCE})")
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
                return False
         return True     
    
    def processSubDirs(self, subDirs):
        pass


    def processFiles(self, files):
        for x in files:
            full = os.path.relpath(self.currDir, self.root) +'/'+x
            if not re.match(".*?\.cp{0,2}$", x):
                continue
            if not self.passFile(x):
                print "ignore file: ", x
                continue
            if self.isTest(self.currDir):
                self.testSource.append(full)
            else:
                self.source.append(full)

    def isTest(self, _dir):
        if re.match(".*?test.*", _dir):
            return True
        return False
    
    def process(self, currDir, subDirs, files):
        if not self.passDir(currDir):
            print "ignore dir: ", currDir
            return
        self.currDir = currDir
        #process
        self.processSubDirs(subDirs)
        self.processFiles(files)

w = Walker(inDir)


