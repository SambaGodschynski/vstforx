cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{CLIBS}/sambag)

project (vstforx)

SET(FRX_VERSION_MAJOR 1)
SET(FRX_VERSION_MINOR 0)
SET(FRX_VERSION_MICRO 3)

execute_process(COMMAND git log --pretty=format:%h -n 1
                OUTPUT_VARIABLE FRX_VERSION_BUILD)

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_definitions(-DBOOST_UBLAS_TYPE_CHECK=0)
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
add_definitions(-D_SCL_SECURE_NO_WARNINGS) #disable warning C4996: 'std::uninitialized_copy' caused by boost::ublas
add_definitions(-D_CRT_SECURE_NO_WARNINGS) #disable strcpy warning in audioeffect.h
#add_definitions(-DSAMBAG_USE_LOG) #BOOST_LOG FAILS WHEN CALLING FROM VST(.dll) SEE #332
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
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O1") #problems with llvm optimization > O1
                                                                      #see minDep branch->testsuite
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O1")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -O1")	
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
	SET(FRX_ASYNC_DSP_TIMER "FRX_TOOLKIT_TIMER")
	SET(FRX_USE_FONTCACHE "0")
ELSEIF(UNIX)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIRS})
	add_definitions(-DDISCO_USE_X11)
	add_definitions(-D__cdecl=__attribute__\(\(__cdecl__\)\)) #aeffect.h: unarm __cdecl
	add_definitions(-DFRX_OS_LINUX)
	SET(FRX_ASYNC_DSP_TIMER "FRX_TOOLKIT_TIMER")
	SET(FRX_USE_FONTCACHE "0")
ELSEIF(WIN32)
        add_definitions(-DDISCO_USE_WIN32)
	add_definitions(-DFRX_OS_WINDOWS)
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd /ZI")
	SET(FRX_ASYNC_DSP_TIMER "FRX_BOOST_TIMER")
	SET(FRX_USE_FONTCACHE "1")
        IF (MSVC)
 	   IF (CMAKE_CL_64)
    	     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
             set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")
	   ENDIF(CMAKE_CL_64)
	ENDIF(MSVC)



ENDIF(APPLE)

find_package(Cairo REQUIRED)
find_package(CppUnitX REQUIRED)
find_package(TinyXMLX REQUIRED)
find_package(LokiX REQUIRED)
find_package(Lua52X REQUIRED)
find_package(Sqlite3X REQUIRED)
find_package(VST23X REQUIRED)
find_package(Sambag REQUIRED)
find_package(BoostX REQUIRED)

INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LOKI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LUA52_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS}/PUBLIC.SDK/SOURCE/VST2.X/)
INCLUDE_DIRECTORIES(${VSTSDK_INCLUDE_DIRS}/PLUGINTERFACES/VST2.X)
INCLUDE_DIRECTORIES(${VSTPLUGIN_INTERFACES_DIRS})
INCLUDE_DIRECTORIES(${SAMBAG_INCLUDE_DIRS})


SET (FRX_CLIBS ${FRX_CLIBS}
	${SAMBAG_LIBRARIES}
	${Boost_LIBRARIES}
	${CPPUNIT_LIBRARIES}
	${CAIRO_LIBRARIES}
	${TINYXML_LIBRARIES}
	${LUA52_LIBRARIES}
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


CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/src/com/FrxConfig.h.in"
  "${PROJECT_BINARY_DIR}/src/com/FrxConfig.h"
)

add_subdirectory(src)
