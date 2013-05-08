import sys
import os
import os.path
import re

inDir = "../src/"

add = """

IF(APPLE)
  SET(FRX_SOURCES ${FRX_SOURCES} ${FRX_MMSOURCES} )
ENDIF(APPLE)

add_library(frx_core ${FRX_SOURCES} ${VSTSDKSOURCE})

SET ( FRX_STANDALONE
        PluginApps/GuiStandalone/Main.cpp
)

add_executable(standalone ${FRX_STANDALONE})
target_link_libraries (standalone frx_core ${FRX_CLIBS})

add_executable(unit_tests ${FRX_TESTSOURCES})
target_link_libraries (unit_tests frx_core ${FRX_CLIBS})

IF(WIN32)
  SET (PlugSources ${PlugSources}
      PluginApps/VSTForx/win/VstForxResourceManager.cpp
      PluginApps/VSTForx/win/VstForxPlugMain.cpp
      PluginApps/VSTForx/win/initResourceMap.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
      PluginApps/VSTForx/win/vstplug.def
      PluginApps/VSTForx/win/resources.rc
   )
ELSEIF(APPLE)
  SET (PlugSources ${PlugSources} ${FRX_RESOURCES}
      PluginApps/VSTForx/mac/VstForxPlugMain.cpp
      PluginApps/VSTForx/mac/VstForxResourceManager.cpp
      PluginApps/VSTForx/mac/CocoaHelper.mm
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
   )
  SET_SOURCE_FILES_PROPERTIES(${FRX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties(PluginApps/VSTForx/mac/CocoaHelper.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
ENDIF(WIN32)


IF (WIN32)
  add_library(vstforx SHARED ${PlugSources})
  target_link_libraries (vstforx frx_core ${FRX_CLIBS})

  add_library(vstforxInstrument SHARED ${PlugSources})
  target_link_libraries (vstforxInstrument frx_core ${FRX_CLIBS})
  set_target_properties(vstforxInstrument PROPERTIES COMPILE_FLAGS -DFRX_IS_INSTRUMENT)

  add_library(vstforxDEMO SHARED ${PlugSources})
  target_link_libraries (vstforxDEMO frx_core ${FRX_CLIBS})
  set_target_properties(vstforxDEMO PROPERTIES COMPILE_FLAGS -DFRX_IS_DEMO)

  add_library(vstforxInstrumentDEMO SHARED ${PlugSources})
  target_link_libraries (vstforxInstrumentDEMO frx_core ${FRX_CLIBS})
  set_target_properties(vstforxInstrumentDEMO PROPERTIES COMPILE_FLAGS "-DFRX_IS_INSTRUMENT -DFRX_IS_DEMO")
ELSEIF(APPLE)
  ADD_EXECUTABLE(vstforx MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforx frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforx PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforx.plist")
  
ENDIF(WIN32)
"""

ignoreDirs = (
    "TestFolders",
    "CMakeFiles",
    #".*test.*",
    ".*GuiStandaloneApp",
     ".*PluginApps",
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
    mmsource = []
    resources = []
    source = []
    def __init__(self, root):
        self.root = root
        
        for currDir, subDirs, files in os.walk(root):
               self.process(currDir, subDirs, files)
               
        self.fHandler = self.createCmakeFile()
        self.writeList("FRX_SOURCES", self.source)
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
                return False
         return True     
    
    def processSubDirs(self, subDirs):
        pass


    def processFiles(self, files):
        for x in files:
            full = os.path.relpath(self.currDir, self.root) +'/'+x
            name, ext = os.path.splitext(x)
            if ext == ".mm":
                self.mmsource.append(full)
            if ext == ".png" and re.match("^images/.*", full):
                self.resources.append(full)
            if not re.match("\.cp{0,2}$", ext):
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


