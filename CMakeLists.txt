cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{CLIBS}/sambag)

project (vstforx)

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_definitions(-DBOOST_UBLAS_TYPE_CHECK=0)
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
#add_definitions(-DSAMBA_WARN_MISSING_IMPL) #show xy not impl. warning

#to avoid name conflicts with existing FindSomeLib.cmake scripts
#every finding script has a "X" postfix: FindSomeLibX.cmake
#(concerns scriptname only)

#UNIX is TRUE on all UNIX-like OS's, including Apple OS X and Cygwin.
#WIN32 is TRUE on Windows, including Cygwin.
#APPLE is TRUE on Apple systems.

IF(APPLE)
	find_package(X11 REQUIRED)
        include_directories(${X11_INCLUDE_DIRS})
	find_package(IConvX REQUIRED)
	add_definitions(-DDISCO_USE_X11)
ELSEIF(UNIX)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIRS})
	add_definitions(-DDISCO_USE_X11)
	add_definitions(-D__cdecl=__attribute__\(\(__cdecl__\)\)) #aeffect.h: unarm __cdecl
ELSEIF(WIN32)
        find_package(LibPngX REQUIRED)
        find_package(ZlibX REQUIRED)
        find_package(PixmanX REQUIRED)
        add_definitions(-DDISCO_USE_WIN32)
ENDIF(APPLE)

find_package(Cairo REQUIRED)
find_package(CppUnitX REQUIRED)
find_package(TinyXMLX REQUIRED)
find_package(LokiX REQUIRED)
find_package(Lua52X REQUIRED)
find_package(Sqlite3X REQUIRED)
find_package(VST23X REQUIRED)
find_package(Sambag REQUIRED)

SET(BOOST_ROOT $ENV{CLIBS}/boost)
SET(Boost_USE_STATIC_LIBS        ON)
#SET(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.49.0 REQUIRED filesystem system thread regex signals locale serialization date_time timer chrono)
include_directories(${CAIRO_INCLUDE_DIRS})
include_directories(${CPPUNIT_INCLUDE_DIRS})
include_directories(${TINYXML_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LOKI_INCLUDE_DIRS})
include_directories(${LUA52_INCLUDE_DIRS})
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${VSTSDK_INCLUDE_DIRS})
include_directories(${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/)
include_directories(${VSTSDK_INCLUDE_DIRS}/pluginterfaces/vst2.x)
include_directories(${VSTPLUGIN_INTERFACES_DIRS})
include_directories(${SAMBAG_INCLUDE_DIRS})


IF(APPLE)
	set (FRX_CLIBS ${FRX_CLIBS}
		${ICONV_LIBRARY_DIRS}/${LIBICONV}
		${X11_LIBRARIES}
	)
ELSEIF(UNIX)
	set (FRX_CLIBS ${FRX_CLIBS}
		${X11_LIBRARIES}
	)
ELSEIF(WIN32)
ENDIF(APPLE)


set (FRX_CLIBS ${FRX_CLIBS}
	${Boost_LIBRARIES}
	${CPPUNIT_LIBRARIES}
	${CAIRO_LIBRARIES}
	${TINYXML_LIBRARIES}
	${LUA52_LIBRARIES}
	${SAMBAG_LIBRARIES}
)

set (VSTSDKSOURCE ${VSTSDKSOURCE}
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/audioeffect.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/audioeffectx.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
)

add_subdirectory(src)
