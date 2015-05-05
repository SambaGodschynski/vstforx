cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{CLIBS}/sambag)


SET(FRX_VERSION_MAJOR 1)
SET(FRX_VERSION_MINOR 0)
SET(FRX_VERSION_MICRO 6)
SET(FRX_VERSION_STR "${FRX_VERSION_MAJOR}.${FRX_VERSION_MINOR}.${FRX_VERSION_MICRO}")

execute_process(COMMAND git --work-tree=${PROJECT_SOURCE_DIR} log --pretty=format:%h -n 1
              OUTPUT_VARIABLE FRX_GITHASH)


IF($ENV{BUILD_NUMBER})
  SET(FRX_VERSION_BUILD $ENV{BUILD_NUMBER})
ELSE($ENV{BUILD_NUMBER})
  SET(FRX_VERSION_BUILD ${FRX_GITHASH})
ENDIF($ENV{BUILD_NUMBER})



execute_process(COMMAND sh getbranch.sh OUTPUT_VARIABLE FRX_VERSION_BRANCH)
project(VSTForx-${FRX_VERSION_BRANCH})


# Common Features
add_definitions(-DFRX_FEATURE_REMOTE_CHANNEL)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFRX_FEATURE_DC_TESTER")

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_definitions(-DBOOST_UBLAS_TYPE_CHECK=0)
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
add_definitions(-D_SCL_SECURE_NO_WARNINGS) #disable warning C4996: 'std::uninitialized_copy' caused by boost::ublas
add_definitions(-D_CRT_SECURE_NO_WARNINGS) #disable strcpy warning in audioeffect.h

IF($ENV{SAMBA_USELOG})
  add_definitions(-DSAMBAG_USE_LOG) #BOOST_LOG FAILS WHEN CALLING FROM VST(.dll) SEE #332
ENDIF($ENV{SAMBA_USELOG})


#add_definitions(-DSAMBA_WARN_MISSING_IMPL) #show xy not impl. warning

#to avoid name conflicts with existing FindSomeLib.cmake scripts
#every finding script has a "X" postfix: FindSomeLibX.cmake
#(concerns scriptname only)

#UNIX is TRUE on all UNIX-like OS's, including Apple OS X and Cygwin.
#WIN32 is TRUE on Windows, including Cygwin.
#APPLE is TRUE on Apple systems.

IF(APPLE)
	find_package(IConvX REQUIRED)
	add_definitions(-DFRX_OS_MAC)
	add_definitions(-DDISCO_USE_COCOA)	
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)	
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	SET(FRX_ASYNC_DSP_TIMER "FRX_BOOST_TIMER")
	SET(FRX_USE_FONTCACHE "0")
	#vst3 fdebug.h want's that
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
ELSEIF(UNIX)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIRS})
	add_definitions(-DDISCO_USE_X11)
	add_definitions(-D__cdecl=__attribute__\(\(__cdecl__\)\)) #aeffect.h: unarm __cdecl
	add_definitions(-DFRX_OS_LINUX)
	SET(FRX_ASYNC_DSP_TIMER "FRX_BOOST_TIMER")
	SET(FRX_USE_FONTCACHE "0")
ELSEIF(WIN32)
        add_definitions(-DDISCO_USE_WIN32)
	add_definitions(-DFRX_OS_WINDOWS)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DNOMINMAX) #prevent collision with std::min/max
	add_definitions(-DSAMBAG_USE_WINDOWS_SHARED_MEMORY)
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /ZI")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	## lnk:out of memory issue fixed with /OPT:NOREF
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:NOREF")
	SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_DLL_LINKER_FLAGS_RELEASE} /OPT:NOREF")
	SET(FRX_ASYNC_DSP_TIMER "FRX_BOOST_TIMER")
	SET(FRX_USE_FONTCACHE "1")

ENDIF(APPLE)

find_package(Cairo REQUIRED)
find_package(CppUnitX REQUIRED)
find_package(TinyXMLX REQUIRED)
find_package(LokiX REQUIRED)
find_package(Lua52X REQUIRED)
find_package(Sqlite3X REQUIRED)
find_package(VST23X REQUIRED)
find_package(VST3X REQUIRED)
find_package(Sambag REQUIRED)
find_package(BoostX REQUIRED)

INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LOKI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LUA52_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SAMBAG_INCLUDE_DIRS})

#vst2
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS}/PUBLIC.SDK/SOURCE/VST2.X/)
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS}/PLUGINTERFACES/VST2.X)
INCLUDE_DIRECTORIES(${VSTPLUGIN_INTERFACES_DIRS})

#vst3
INCLUDE_DIRECTORIES(${VSTSDK3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst)
INCLUDE_DIRECTORIES(${VSTSDK3_INCLUDE_DIRS}/pluginterfaces/vst)


SET (FRX_CLIBS ${FRX_CLIBS}
	${SAMBAG_LIBRARIES}
	${Boost_LIBRARIES}
	${CPPUNIT_LIBRARIES}
	${CAIRO_LIBRARIES}
	${TINYXML_LIBRARIES}
	${LUA52_LIBRARIES}
	#${VST3_LIBRARIES}
)

IF(APPLE)
	set (FRX_CLIBS ${FRX_CLIBS}
		${COCOA_LIBRARY}
                ${CARBON_LIBRARY}			

	)
ELSEIF(UNIX)
	set (FRX_CLIBS ${FRX_CLIBS}
		${X11_LIBRARIES}
		rt #undefined reference to `clock_gettime'
	)
ELSEIF(WIN32)
ENDIF(APPLE)



set (VSTSDKSOURCE ${VSTSDKSOURCE}
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/audioeffect.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/audioeffectx.cpp
      #${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp now part of target sources
)

set (VSTSDK3SOURCE ${VSTSDK3SOURCE}
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstaudioeffect.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstbus.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstcomponent.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstcomponentbase.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vsteditcontroller.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstinitiids.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/vst/vstparameters.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/common/pluginview.cpp
      ${VSTSDK3_INCLUDE_DIRS}/public.sdk/source/common/memorystream.cpp      
      ${VSTSDK3_INCLUDE_DIRS}/pluginterfaces/base/ustring.cpp
      ${VSTSDK3_INCLUDE_DIRS}/pluginterfaces/base/conststringtable.cpp
      ${VSTSDK3_INCLUDE_DIRS}/pluginterfaces/base/funknown.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/baseiids.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/classfactory.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fatomic.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fbitset.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fbuffer.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fcpu.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fdebug.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fdynlib.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/finitializer.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fmemory.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fobject.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fpoint.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/frect.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fregion.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fstreamer.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fstring.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/fthread.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/timer.cpp
      ${VSTSDK3_INCLUDE_DIRS}/base/source/updatehandler.cpp
)



CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/src/com/FrxConfig.h.in"
  "${PROJECT_BINARY_DIR}/src/com/FrxConfig.h"
)

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/builds/currVersion.txt.in"
  "${PROJECT_BINARY_DIR}/builds/currVersion.txt"
)


add_subdirectory(src)
