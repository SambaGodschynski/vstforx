cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(vstforx VERSION 1.0.72 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Options ───────────────────────────────────────────────────────────────────
option(FRX_BUILD_STANDALONE "Build standalone application" ON)
option(FRX_BUILD_PLUGIN     "Build VST plugin shared library" OFF)
option(FRX_BUILD_VST3       "Include VST3 hosting support (requires VST3 SDK)" OFF)
option(FRX_USE_LOG          "Enable boost::log" OFF)

# ── Version / config ──────────────────────────────────────────────────────────
set(FRX_VERSION_MAJOR 1)
set(FRX_VERSION_MINOR 0)
set(FRX_VERSION_MICRO 72)

execute_process(
    COMMAND git --work-tree=${PROJECT_SOURCE_DIR} log --pretty=format:%h -n 1
    OUTPUT_VARIABLE FRX_GITHASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT FRX_GITHASH)
    set(FRX_GITHASH "unknown")
endif()
set(FRX_VERSION_BUILD "${FRX_GITHASH}")
set(FRX_VERSION_BRANCH "main")

# ── VST2 SDK (bundled in sambag submodule) ────────────────────────────────────
# When built as add_subdirectory from the workspace root, CMAKE_SOURCE_DIR
# points to the workspace root where sambag lives.
set(FRX_VSTSDK2_DIR "${CMAKE_SOURCE_DIR}/sambag/ext/vstsdk2.4"
    CACHE PATH "VST2 SDK root")

if(NOT EXISTS "${FRX_VSTSDK2_DIR}/pluginterfaces/vst2.x/aeffect.h")
    message(FATAL_ERROR
        "VST2 SDK not found at FRX_VSTSDK2_DIR=${FRX_VSTSDK2_DIR}\n"
        "Expected: ${FRX_VSTSDK2_DIR}/pluginterfaces/vst2.x/aeffect.h")
endif()

# ── Platform ──────────────────────────────────────────────────────────────────
if(APPLE)
    add_compile_definitions(DISCO_USE_COCOA FRX_OS_MAC RELEASE)
    find_library(COCOA_LIBRARY  Cocoa  REQUIRED)
    find_library(CARBON_LIBRARY Carbon REQUIRED)
    set(FRX_ASYNC_DSP_TIMER 1)
    set(FRX_USE_FONTCACHE 0)
elseif(UNIX)
    find_package(X11 REQUIRED)
    add_compile_definitions(DISCO_USE_X11 FRX_OS_LINUX)
    # aeffect.h uses __cdecl which GCC doesn't know without this
    add_compile_definitions("__cdecl=__attribute__((__cdecl__))")
    set(FRX_ASYNC_DSP_TIMER 1)
    set(FRX_USE_FONTCACHE 0)
elseif(WIN32)
    add_compile_definitions(
        DISCO_USE_WIN32
        FRX_OS_WINDOWS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        SAMBAG_USE_WINDOWS_SHARED_MEMORY
        _WIN32_WINNT=0x0A00
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(FRX_ASYNC_DSP_TIMER 1)
    set(FRX_USE_FONTCACHE 1)
endif()

add_compile_definitions(
    BOOST_UBLAS_TYPE_CHECK=0
    BOOST_BIND_GLOBAL_PLACEHOLDERS
    BOOST_FILESYSTEM_VERSION=3
    _SCL_SECURE_NO_WARNINGS
    _CRT_SECURE_NO_WARNINGS
    FRX_FEATURE_REMOTE_CHANNEL
)
if(FRX_USE_LOG)
    add_compile_definitions(SAMBAG_USE_LOG FRX_USE_LOG)
endif()

# ── Dependencies ──────────────────────────────────────────────────────────────
if(NOT WIN32)
    find_package(PkgConfig REQUIRED)
endif()

# Boost
set(_BOOST_COMPS filesystem system thread regex serialization
                 date_time timer chrono program_options)
if(FRX_USE_LOG)
    list(APPEND _BOOST_COMPS log log_setup)
endif()
find_package(Boost 1.55.0 REQUIRED COMPONENTS ${_BOOST_COMPS})

# Cairo
if(WIN32)
    find_path(CAIRO_INCLUDE_DIR NAMES cairo/cairo.h REQUIRED)
    find_library(CAIRO_LIBRARY  NAMES cairo         REQUIRED)
    find_library(PIXMAN_LIBRARY NAMES pixman-1       REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(ZLIB     REQUIRED)
    find_package(PNG      REQUIRED)
    find_library(FONTCONFIG_LIBRARY NAMES fontconfig)
    find_package(EXPAT REQUIRED)
    set(FRX_CAIRO_LIBRARIES
        "${CAIRO_LIBRARY}" "${PIXMAN_LIBRARY}"
        Freetype::Freetype ZLIB::ZLIB PNG::PNG
    )
    if(FONTCONFIG_LIBRARY)
        list(APPEND FRX_CAIRO_LIBRARIES "${FONTCONFIG_LIBRARY}" EXPAT::EXPAT)
    endif()
else()
    pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)
    set(FRX_CAIRO_LIBRARIES PkgConfig::Cairo)
endif()

# Lua
if(WIN32)
    find_package(Lua REQUIRED)
    add_library(_frx_lua INTERFACE)
    target_include_directories(_frx_lua INTERFACE ${LUA_INCLUDE_DIR})
    target_link_libraries(_frx_lua INTERFACE ${LUA_LIBRARIES})
    set(FRX_LUA_LIBRARIES _frx_lua)
else()
    pkg_check_modules(Lua lua5.4 IMPORTED_TARGET)
    if(NOT Lua_FOUND)
        pkg_check_modules(Lua lua5.2 IMPORTED_TARGET)
    endif()
    if(NOT Lua_FOUND)
        pkg_check_modules(Lua REQUIRED lua IMPORTED_TARGET)
    endif()
    set(FRX_LUA_LIBRARIES PkgConfig::Lua)
endif()

# ── Configure generated header (after platform vars are set) ─────────────────
configure_file(
    "${PROJECT_SOURCE_DIR}/src/com/FrxConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src/com/FrxConfig.h"
)

# ── Subdir ────────────────────────────────────────────────────────────────────
add_subdirectory(src)
